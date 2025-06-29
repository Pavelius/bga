#include "animation.h"
#include "area.h"
#include "calendar.h"
#include "console.h"
#include "container.h"
#include "creature.h"
#include "door.h"
#include "draw.h"
#include "drawable.h"
#include "floattext.h"
#include "game.h"
#include "math.h"
#include "region.h"
#include "resid.h"
#include "resinfo.h"
#include "timer.h"
#include "vector.h"
#include "view.h"
#include "worldmap.h"

using namespace draw;

static worldmapi::area* current_world_area_hilite;
static vector<item*> container_items, items;

void paperdoll(color* pallette, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm);
void scale2x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned width, unsigned height);

const int tile_size = 64;

static point hotspot;
static rect last_screen, last_area;
static int zoom_factor = 1;

static unsigned get_game_tick() {
	return current_tick / 64;
}

static void correct_camera() {
	if(camera.x + last_screen.width() > area_width * 16)
		camera.x = area_width * 16 - last_screen.width();
	if(camera.x < 0)
		camera.x = 0;
	if(camera.y + last_screen.height() > area_height_tiles * 16)
		camera.y = area_height_tiles * 16 - last_screen.height();
	if(camera.y < 0)
		camera.y = 0;
}

void setcamera(point v) {
	camera.x = v.x - last_screen.width() / 2;
	camera.y = v.y - last_screen.height() / 2;
	correct_camera();
}

static void actor_marker(int size, bool flicking, bool double_border) {
	auto r = size * 6 + 4;
	if(flicking)
		r += iabs(int((current_tick / 100) % 6) - 3) - 1;
	circle(r);
	if(double_border)
		circle(r + 1);
}

static void paint_tiles() {
	auto sp = get_area_sprites();
	if(!sp)
		return;
	int tx0 = camera.x / tile_size, ty0 = camera.y / tile_size;
	int dx = width / tile_size + 1, dy = height / tile_size + 1;
	int tx1 = tx0 + dx, ty1 = ty0 + dy;
	if(tx1 > area_width / 4 - 1)
		tx1 = area_width / 4 - 1;
	if(ty1 > area_height_tiles / 4 - 1)
		ty1 = area_height_tiles / 4 - 1;
	int ty = ty0;
	while(ty <= ty1) {
		int tx = tx0;
		while(tx <= tx1) {
			auto x = last_screen.x1 + tx * tile_size - camera.x;
			auto y = last_screen.y1 + ty * tile_size - camera.y;
			draw::image(x, y, sp, area_tiles[ty * 64 + tx], 0);
			tx++;
		}
		ty++;
	}
}

static void paint_block_area() {
	static bool show;
	if(hot.key == Ctrl + 'B')
		show = !show;
	if(!show)
		return;
	int tx0 = camera.x / 16, ty0 = camera.y / 12;
	int tx1 = tx0 + width / 16 + 1, ty1 = ty0 + height / 12 + 1;
	if(tx1 > area_width - 1)
		tx1 = area_width - 1;
	if(ty1 > area_height - 1)
		ty1 = area_height - 1;
	width = 16 - 2; height = 12 - 2;
	pushfore push_fore(colors::black);
	auto push_alpha = alpha; alpha = 64;
	for(auto ty = ty0; ty < ty1; ty++) {
		for(auto tx = tx0; tx < tx1; tx++) {
			caret.x = tx * 16 - camera.x + 1;
			caret.y = ty * 12 - camera.y + 1;
			auto a = area_cost[m2i(tx, ty)];
			if(a < Blocked)
				continue;
			switch(a) {
			case Blocked: fore = colors::black; break;
			case BlockedCreature: fore = colors::blue; break;
			case BlockedLeft: case BlockedUp: fore = colors::yellow; break;
			}
			rectf();
		}
	}
	alpha = push_alpha;
	fore = colors::red;
	point pt = point(hotspot.x / 16, hotspot.y / 12);
	caret.x = pt.x * 16 - camera.x;
	caret.y = pt.y * 12 - camera.y;
	width++; height++;
	rectb();
	fore = colors::white;
	auto index = m2i(pt.x, pt.y);
	auto blocked = is_block(index);
	char temp[512]; stringbuilder sb(temp);
	sb.add("%1i, %2i index = %3i", pt.x, pt.y, index);
	sb.adds("(spot %1i, %2i)", hotspot.x, hotspot.y);
	if(is_block(index))
		sb.adds("blocked");
	setcaret(4, 410); text(temp);
}

static point camera_center() {
	return center(last_area);
}

void change_zoom_factor() {
	auto pt = camera_center();
	auto v1 = zoom_factor;
	zoom_factor = (zoom_factor != 2) ? 2 : 1;
	auto v2 = zoom_factor;
	last_screen.x2 = last_screen.x1 + last_screen.width() * v1 / v2;
	last_screen.y2 = last_screen.y1 + last_screen.height() * v1 / v2;
	setcamera(pt);
}

void change_zoom_factor(int bonus) {
	change_zoom_factor();
}

static void set_standart_cursor() {
	cursor.set(CURSORS, 0);
	if(hot.mouse.in(last_screen)) {
		if(!combat_mode || area_cost[s2i(hotspot)] < Blocked)
			cursor.set(CURSORS, 4);
	}
}

static void apply_shifer() {
	rect screen = {0, 0, getwidth(), getheight()};
	int index = -1;
	const int sz = 4;
	auto d = hot.mouse;
	if(d.x <= screen.x1)
		d.x = screen.x1;
	else if(d.x >= screen.x2 - 1)
		d.x = screen.x2 - 1;
	if(d.y <= screen.y1)
		d.y = screen.y1;
	else if(d.y >= screen.y2 - 1)
		d.y = screen.y2 - 1;
	if(d.x <= screen.x1 + sz)
		index = (d.y <= screen.y1 + sz) ? 3 : (d.y < screen.x2 - sz) ? 4 : 5;
	else if(d.x >= screen.x2 - sz)
		index = (d.y <= screen.y1 + sz) ? 7 : (d.y <= screen.y2 - sz) ? 0 : 1;
	else
		index = (d.y <= screen.x1 + sz) ? 2 : (d.y <= screen.y2 - sz) ? -1 : 6;
	if(index == -1)
		return;
	const int camera_step = 16;
	cursor.set(CURSARW, index);
	switch(index) {
	case 0: camera.x += camera_step; break;
	case 2: camera.y -= camera_step; break;
	case 4: camera.x -= camera_step; break;
	case 6: camera.y += camera_step; break;
	}
	correct_camera();
}

static void rectblack(rect rc) {
	pushrect push;
	pushfore push_fore(colors::black);
	caret.x = rc.x1;
	caret.y = rc.y1;
	width = rc.width();
	height = rc.height();
	rectf();
}

static void setup_visible_area() {
	auto push_caret = caret;
	auto mx = (area_width / 4) * tile_size;
	auto my = (area_height / 4) * tile_size;
	if(mx < width) {
		caret.x += (width - mx) / 2;
		rectblack({push_caret.x, push_caret.y, caret.x, push_caret.y + height});
		rectblack({caret.x + mx, push_caret.y, caret.x + width, push_caret.y + height});
	} else
		mx = width;
	if(my < height) {
		caret.y += (height - my) / 2;
		rectblack({push_caret.x, push_caret.y, caret.x+width, caret.y});
		rectblack({push_caret.x, caret.y + my, caret.x + width, caret.y + height});
	} else
		my = height;
	last_screen.set(caret.x, caret.y, caret.x + mx, caret.y + my);
	last_area = last_screen; last_area.move(camera.x, camera.y);
	last_area.offset(-128, -128);
	if(hot.mouse.in(last_screen))
		hotspot = hot.mouse - caret + camera;
	else
		hotspot = {-1000, -1000};
}

static void update_floattext_tail() {
	auto pb = bsdata<floattext>::begin();
	auto pe = bsdata<floattext>::end();
	while(pe > pb) {
		pe--;
		if(*(pe))
			break;
		bsdata<floattext>::source.count--;
	}
}

static void prepare_creatures() {
	for(auto& e : bsdata<creature>()) {
		if(!e.ispresent())
			continue;
		if(!e.position.in(last_area))
			continue;
		objects.add(&e);
	}
}

static void prepare_floattext() {
	for(auto& e : bsdata<floattext>()) {
		if(!e)
			continue;
		if(e.stop_visible < current_game_tick) {
			e.clear();
			continue;
		}
		if(!e.position.in(last_area))
			continue;
		objects.add(&e);
	}
}

static void prepare_animation() {
	for(auto& e : bsdata<animation>()) {
		if(!e.position.in(last_area))
			continue;
		if(!e.isvisible())
			continue;
		objects.add(&e);
	}
}

static void prepare_containers() {
	for(auto& e : bsdata<container>()) {
		if(!e.position.in(last_area))
			continue;
		objects.add(&e);
	}
}

static void prepare_regions() {
	for(auto& e : bsdata<region>()) {
		if(e.type == RegionTriger)
			continue;
		if(!e.position.in(last_area))
			continue;
		objects.add(&e);
	}
}

static void prepare_doors() {
	for(auto& e : bsdata<door>()) {
		if(!e.position.in(last_area))
			continue;
		objects.add(&e);
	}
}

static void prepare_objects() {
	objects.clear();
	prepare_animation();
	prepare_containers();
	prepare_creatures();
	prepare_doors();
	prepare_floattext();
	prepare_regions();
	update_floattext_tail();
}

static void sort_objects() {
	objects.sort(drawable::compare);
}

static void polygon(const sliceu<point>& source) {
	auto pb = source.begin();
	auto pe = source.end();
	if(pb >= pe)
		return;
	caret = pb[0] - camera;
	for(auto p = pb + 1; p < pe; p++)
		line(p->x - camera.x, p->y - camera.y);
	line(pb->x - camera.x, pb->y - camera.y);
}

static void polygon_green(const sliceu<point>& source) {
	auto push_fore = fore;
	fore = colors::green;
	polygon(source);
	fore = push_fore;
}

static void polygon_red(const sliceu<point>& source) {
	auto push_fore = fore;
	fore = colors::red;
	polygon(source);
	fore = push_fore;
}

static void apply_shadow(color* pallette, color fore) {
	for(auto i = 0; i < 256; i++)
		pallette[i] = pallette[i] * fore;
}

static void paint_markers(const creature* p) {
	auto push_fore = fore;
	fore = p->isparty() ? colors::green : colors::red;
	if(p->ishilite())
		actor_marker(p->getsize(), true, player == p);
	else if(p->isselected())
		actor_marker(p->getsize(), false, player == p);
	fore = push_fore;
}

static void paint_creature(const drawable* object) {
	auto p = (creature*)object;
	if(p->ishilite())
		cursor.cicle = 0;
	paint_markers(p);
	p->paint();
}

static void paint_animation(const drawable* object) {
	auto p = (animation*)object;
	auto pr = gres(p->rsname, "art/animations");
	if(!pr)
		return;
	auto hour = gethour();
	if(p->is(RenderBlackAsTransparent)) {
		// image_tint(caret.x, caret.y, pr, pr->ganim(frame, get_game_tick()), is(Mirrored) ? ImageMirrorV : 0);
		auto push_alpha = alpha;
		alpha = alpha >> 2;
		image(pr, pr->ganim(p->frame, get_game_tick()), p->is(Mirrored) ? ImageMirrorV : 0);
		alpha = push_alpha;
	} else
		image(pr, pr->ganim(p->frame, get_game_tick()), p->is(Mirrored) ? ImageMirrorV : 0);
}

static void paint_float_text(const drawable* object) {
	draw::pushrect push;
	auto p = (floattext*)object;
	auto push_fore = draw::fore;
	auto push_alpha = draw::alpha;
	draw::width = p->box.width();
	draw::height = p->box.height();
	draw::fore = colors::black;
	draw::alpha = 128;
	draw::strokeout(draw::rectf, metrics::border + metrics::padding);
	draw::alpha = push_alpha;
	draw::fore = push_fore;
	draw::textf(p->format);
}

static void paint_door(const drawable* object) {
	auto p = (door*)object;
	if(p->ishilite()) {
		polygon_green(p->getpoints());
		cursor.cicle = p->cursor;
	}
}

static void paint_region(const drawable* object) {
	auto p = (region*)object;
	if(p->ishilite()) {
		switch(p->type) {
		case RegionInfo: cursor.cicle = 22; break;
		case RegionTravel: cursor.cicle = 34; break;
		}
	}
}

static void paint_container(const drawable* object) {
	auto p = (container*)object;
	if(p->ishilite()) {
		polygon_green(p->points);
		cursor.cicle = 2;
	}
}

static void paint_object(drawable* object) {
	if(bsdata<door>::have(object))
		paint_door(object);
	else if(bsdata<region>::have(object))
		paint_region(object);
	else if(bsdata<container>::have(object))
		paint_container(object);
	else if(bsdata<floattext>::have(object))
		paint_float_text(object);
	else if(bsdata<creature>::have(object))
		paint_creature(object);
	else if(bsdata<animation>::have(object))
		paint_animation(object);
}

static bool is_hilite(const drawable* object) {
	if(bsdata<door>::have(object)) {
		auto p = (door*)object;
		if(hotspot.in(p->box)) {
			auto n = p->getpoints();
			return inside(hotspot, n.begin(), n.size());
		}
	} else if(bsdata<region>::have(object)) {
		auto p = (region*)object;
		if(hotspot.in(p->box))
			return inside(hotspot, p->points.begin(), p->points.size());
	} else if(bsdata<container>::have(object)) {
		auto p = (container*)object;
		if(hotspot.in(p->box))
			return inside(hotspot, p->points.begin(), p->points.size());
	} else if(bsdata<creature>::have(object)) {
		auto p = (creature*)object;
		return hotspot.in(p->getbox());
	}
	return false;
}

static void paint_objects() {
	auto push_caret = caret;
	hilite_drawable = 0;
	for(auto p : objects) {
		caret = p->position - camera;
		caret.x += last_screen.x1;
		caret.y += last_screen.y1;
		if(is_hilite(p))
			hilite_drawable = p;
		paint_object(p);
	}
	caret = push_caret;
}

static const char* gettipsname(point position) {
	return str("%3Info%1i_%2i", position.x, position.y, area_name);
}

static void apply_hilite_command() {
	if(!hilite_drawable)
		return;
	if(hot.key == MouseLeft && !hot.pressed) {
		if(bsdata<region>::have(hilite_drawable)) {
			auto p = (region*)hilite_drawable;
			if(p->type == RegionInfo) {
				auto pn = getnme(gettipsname(p->position));
				if(pn) {
					add_float_text(hotspot, pn, 320, 1000 * 5, p);
					print("[+%1]", pn);
				}
			} else if(p->type == RegionTravel)
				enter(p->move_to_area, p->move_to_entrance);
		} else if(bsdata<door>::have(hilite_drawable)) {
			auto p = (door*)hilite_drawable;
			p->use(!p->isopen());
		} else if(bsdata<container>::have(hilite_drawable)) {
			auto p = (container*)hilite_drawable;
			print("This is %1 (vairable %2i)", p->name, p->variable_index);
			player->moveto(p->launch);
		} else if(bsdata<creature>::have(hilite_drawable)) {
			if(combat_mode) {

			} else
				execute(choose_creature, 0, 0, hilite_drawable);
		}
	}
}

static void jump_party() {
	setparty(hot.param);
}

static void move_party() {
	party_move(hot.param);
}

static void apply_command() {
	if(hilite_drawable)
		return;
	if(hot.mouse.in(last_screen)) {
		if(hot.pressed) {
			auto rc = create_rect(left_mouse_press + camera, hot.mouse + camera);
			if(rc.size() >= 8) {
				pushrect push;
				pushfore push_fore(colors::green);
				cursor.set(NONE, 0);
				caret.x = rc.x1 - camera.x;
				caret.y = rc.y1 - camera.y;
				width = rc.width();
				height = rc.height();
				rectb();
			}
		}
		if(hot.key == MouseLeft && !hot.pressed) {
			auto rc = create_rect(left_mouse_press + camera, hot.mouse + camera);
			if(rc.size() >= 8) {
				clear_selection();
				for(auto p : party) {
					if(!p)
						continue;
					if(p->position.in(rc))
						p->select();
				}
			} else
				execute(move_party, hotspot);
		}
	}
}

static void apply_command_combat() {
	if(hilite_drawable)
		return;
	if(!hot.mouse.in(last_screen))
		return;
	auto map_index = s2i(hotspot);
	if(area_cost[map_index] >= Blocked) {
		cursor.set(CURSORS, 6);
		return;
	}
	if(hot.key == MouseLeft && !hot.pressed)
		execute(buttonparam, (long)(area_cost + map_index));
}

static void paint_movement_target() {
	if(!combat_mode)
		return;
	int tx0 = camera.x / 16, ty0 = camera.y / 12;
	int tx1 = tx0 + width / 16 + 1, ty1 = ty0 + height / 12 + 1;
	if(tx1 > area_width - 1)
		tx1 = area_width - 1;
	if(ty1 > area_height - 1)
		ty1 = area_height - 1;
	width = 16 - 2; height = 12 - 2;
	// width = 16; height = 12;
	pushfore push_fore;
	auto push_alpha = alpha; alpha = 64;
	for(auto ty = ty0; ty < ty1; ty++) {
		for(auto tx = tx0; tx < tx1; tx++) {
			caret.x = tx * 16 - camera.x + 1;
			caret.y = ty * 12 - camera.y + 1;
			auto a = get_cost(m2i(tx, ty));
			switch(a) {
			case Blocked: continue;
			case BlockedCreature: fore = colors::black; break;
			default: fore = colors::black; break;
			}
			rectf();
		}
	}
	alpha = push_alpha;
}

static void paint_area_map() {
	auto push_clip = clipping; setclipall();
	setup_visible_area();
	set_standart_cursor();
	paint_tiles();
	paint_movement_target();
#ifdef _DEBUG
	paint_block_area();
#endif // _DEBUG
	prepare_objects();
	sort_objects();
	paint_objects();
	apply_hilite_command();
	if(combat_mode)
		apply_command_combat();
	else
		apply_command();
	clipping = push_clip;
}

static void paint_area_map_spot() {
	pushrect push;
	auto push_clip = clipping; setclipall();
	cursor.set(CURSORS, 0);
	setup_visible_area();
	paint_tiles();
	paint_movement_target();
	prepare_objects();
	sort_objects();
	paint_objects();
	clipping = push_clip;
}

static void paint_area_map_zoom_factor(fnevent proc) {
	auto push_clipping = clipping;
	auto push_mouse = hot.mouse; hot.mouse.x /= zoom_factor; hot.mouse.y /= zoom_factor;
	pushrect push; width /= zoom_factor; height /= zoom_factor;
	static surface temporary_canvas; temporary_canvas.resize(width, height, 32, true);
	auto push_canvas = canvas;
	canvas = &temporary_canvas; setclip();
	proc();
	canvas = push_canvas;
	if(zoom_factor == 2) {
		scale2x(canvas->ptr(push.caret.x, push.caret.y), canvas->scanline,
			temporary_canvas.ptr(0, 0), temporary_canvas.scanline,
			width, height);
	}
	hot.mouse = push_mouse;
	clipping = push_clipping;
}

static void paint_area_map_zoomed(fnevent proc) {
	if(zoom_factor <= 1)
		proc();
	else
		paint_area_map_zoom_factor(proc);
}

static void apply_hotkeys() {
	switch(hot.key) {
	case 'G': execute(game_quick_save); break;
	}
}

void paint_area() {
	paint_area_map_zoomed(paint_area_map);
	apply_shifer();
	apply_hotkeys();
}

static point minimap_origin, minimap_size;

static point m2mm(point mm) {
	mm.x = mm.x / 8;
	mm.y = mm.y / 8;
	return minimap_origin + mm;
}

static point mm2m(point m) {
	m = m - minimap_origin;
	m.x *= 8;
	m.y *= 8;
	return m;
}

static void set_camera() {
	setcamera({(short)hot.param, (short)hot.param2});
}

void paint_minimap() {
	if(last_screen.x2 == 0) {
		last_screen.x2 = 800;
		last_screen.y2 = 433;
	}
	pushrect push;
	// Minimap image
	auto mm = get_minimap();
	if(!mm)
		return;
	auto& sf = mm->get(0);
	caret.x += (width - sf.sx) / 2;
	caret.y += (height - sf.sy) / 2;
	width = sf.sx; height = sf.sy;
	minimap_origin = caret;
	minimap_size.x = sf.sx;
	minimap_size.y = sf.sy;
	image(mm, 0, 0);
	// cursor = default_cursor;
	if(ishilite()) {
		cursor.cicle = 44;
		if(hot.key == MouseLeft && hot.pressed) {
			auto np = mm2m(hot.mouse);
			execute(set_camera, np.x, np.y, 0);
		}
	}
	// Screen rect
	caret = m2mm(camera);
	point cameral = camera;
	cameral.x += last_screen.width();
	cameral.y += last_screen.height();
	cameral = m2mm(cameral);
	width = cameral.x - caret.x;
	height = cameral.y - caret.y;
	rectb();
	// Party position
	auto push_fore = fore;
	fore = colors::green;
	for(auto p : party) {
		if(!p->ispresent())
			continue;
		caret = m2mm(p->position);
		circle(2);
	}
	fore = push_fore;
}

static void paint_area_map_screen() {
	pushrect push;
	setcaret(0, 0, 800, 600);
	auto push_clip = clipping; setclip();
	setup_visible_area();
	paint_tiles();
	prepare_objects();
	sort_objects();
	paint_objects();
	clipping = push_clip;
}

void get_save_screenshoot(surface& sm) {
	auto push_last_screen = last_screen;
	paint_area_map_screen();
	blit(sm, 0, 0, sm.width, sm.height, 0, *canvas, 0, 0, canvas->width, canvas->height);
	last_screen = push_last_screen;
}

static void enter_current_world_area() {
	auto p = (worldmapi::area*)hot.object;
	if(p)
		enter(p->id, 0);
}

void paint_worldmap_area() {
	current_world_area_hilite = 0;
	if(!current_world)
		return;
	auto push_clip = clipping; setclipall();
	auto back = current_world->background->get();
	image(caret.x, caret.y, back, 0, 0);
	auto icons = current_world->icons->get();
	if(!icons)
		return;
	if(ishilite())
		cursor.cicle = 44;
	auto push_caret = caret;
	auto current_party_area = get_party_world_area();
	for(auto& e : bsdata<worldmapi::area>()) {
		if(e.realm != current_world)
			continue;
		//if(!e.is(AreaVisible))
		//	continue;
		caret = push_caret + e.position;
		auto& f = icons->get(e.avatar);
		caret.x -= f.sx / 2;
		caret.y -= f.sy / 2;
		image(icons, e.avatar, 0);
		if(current_party_area == &e)
			image(icons, 22, 0);
		fore = colors::white;
		if(e.isinteract()) {
			if(hot.mouse.in({caret.x - 2, caret.y - 2, caret.x + f.sx + 2, caret.y + f.sy + texth() + 2}))
				current_world_area_hilite = &e;
		} else
			fore = fore.mix(colors::black, 128);
		if(current_world_area_hilite == &e) {
			cursor.cicle = 34;
			fore = colors::yellow;
			if(hot.key == MouseLeft && !hot.pressed)
				execute(enter_current_world_area, 0, 0, current_world_area_hilite);
		}
		auto name = e.getname();
		auto w = textw(name);
		caret.x -= (w - f.sx) / 2;
		caret.y += f.sy;
		text(name, -1, TextStroke);
	}
	caret = push_caret;
	clipping = push_clip;
}

static void paint_choose_order() {
	update_frames();
	setcaret(0, 0, 800, 433);
	paint_area();
	paint_action_panel_combat();
	paint_game_panel(false, true);
}

void open_combat_mode() {
	auto push_combat = combat_mode; combat_mode = true;
	clear_selection();
	player->select();
	clear_path_map();
	block_creatures();
	create_wave(player->position_index, player->getsize());
	block_movement(player->getmovement() * 2);
	scene(paint_choose_order);
	combat_mode = push_combat;
}

void* choose_combat_action() {
	open_combat_mode();
	return (void*)getresult();
}

static void paint_container() {
	paint_game_dialog(0, 476, GUICONT, 1);
	setdialog(62, 25); image(gres(CONTAINER), 3, 0);
	setdialog(430, 28); image(gres(CONTAINER), 1, 0);

	setdialog(150, 22); button(STONSLOT, 0, 0);
	setdialog(195, 22); button(STONSLOT, 0, 0);
	setdialog(239, 22); button(STONSLOT, 0, 0);
	setdialog(283, 22); button(STONSLOT, 0, 0);
	setdialog(327, 22); button(STONSLOT, 0, 0);
	setdialog(150, 65); button(STONSLOT, 0, 0);
	setdialog(195, 65); button(STONSLOT, 0, 1);
	setdialog(239, 65); button(STONSLOT, 0, 1);
	setdialog(283, 65); button(STONSLOT, 0, 1);
	setdialog(327, 65); button(STONSLOT, 0, 1);
	//Scroll GBTNSCRL 375 24 12 76 frames(1 0 3 2 4 5)

	setdialog(509, 22); button(STONSLOT, 0, 0);
	setdialog(553, 22); button(STONSLOT, 0, 0);
	setdialog(553, 65); button(STONSLOT, 0, 0);
	setdialog(509, 65); button(STONSLOT, 0, 0);
	//Scroll GBTNSCRL 602 24 12 76 frames(1 0 3 2 4 5)

	setdialog(661, 78, 70, 20); texta(str("%1i", player->coins), AlignRightCenter);
	setdialog(684, 28); button(GBTNOPT1, 1, 2, KeyEscape); fire(buttoncancel);
}

static void mouse_area_cancel() {
	if(hot.key == MouseLeft && !hot.pressed && ishilite())
		execute(buttoncancel);
}

static void paint_container_area() {
	update_frames();
	setcaret(0, 0, 800, 476); paint_area_map_zoomed(paint_area_map_spot); mouse_area_cancel();
	paint_container();
}

void open_container() {
	scene(paint_container_area);
}