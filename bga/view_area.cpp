#include "animation.h"
#include "area.h"
#include "audio.h"
#include "calendar.h"
#include "console.h"
#include "container.h"
#include "creaturea.h"
#include "door.h"
#include "draw.h"
#include "drawable.h"
#include "floattext.h"
#include "game.h"
#include "itemground.h"
#include "keybind.h"
#include "math.h"
#include "order.h"
#include "pushvalue.h"
#include "region.h"
#include "resinfo.h"
#include "rfiles.h"
#include "timer.h"
#include "vector.h"
#include "view.h"
#include "worldmap.h"

using namespace draw;

typedef bool(*fnrenderallow)(const drawable* p);
typedef int(*fnrenderget)(const drawable* p);

struct renderi : nameable {
	const array&	source;
	drawable*		elements;
	fnevent			paint;
	fnrenderallow	allow;
	fnrenderget		priority;
};

struct drawobject {
	drawable* object;
	renderi* render;
};

static color pallette[256];
static worldmapi::area* current_world_area_hilite;
static vector<item*> container_items, items;
static adat<drawobject> objects;
static rect clipped_area;

void scale2x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned width, unsigned height);

const int tile_size = 64;

static point hotspot;
static rect last_screen, last_area;
static int zoom_factor = 1;

static bool allow_clipped_area(const drawable* p) {
	return p->position.in(clipped_area);
}

static bool allow_region(const drawable* object) {
	auto p = (region*)object;
	if(p->type == RegionTriger)
		return false;
	return p->box.intersect(last_area);
}

static bool allow_container(const drawable* object) {
	auto p = (container*)object;
	return p->box.intersect(last_area);
}

static bool allow_door(const drawable* object) {
	auto p = (door*)object;
	return p->getrect().intersect(last_area);
}

static bool allow_animate(const drawable* object) {
	auto p = (animation*)object;
	if(!p->isvisible())
		return false;
	return allow_clipped_area(object);
}

static bool allow_ground(const drawable* object) {
	auto p = (itemground*)object;
	if(p->area != current_area)
		return false;
	if(p->inside())
		return false;
	return allow_clipped_area(object);
}

static void add_object(renderi* pm, drawable* p) {
	auto pn = objects.add();
	pn->object = p;
	pn->render = pm;
}

static void add_objects(renderi* pm) {
	if(!pm->allow)
		return;
	auto sz = pm->source.element_size;
	auto pe = (unsigned char*)pm->elements + sz * pm->source.count;
	for(auto p = pm->elements; (unsigned char*)p < pe; p = (drawable*)((char*)p + sz)) {
		if(!pm->allow(p))
			continue;
		auto pn = objects.add();
		pn->object = p;
		pn->render = pm;
	}
}

static void paint_float_text(floattext* p) {
	pushrect push;
	auto push_fore = draw::fore;
	auto push_alpha = draw::alpha;
	width = p->box.width();
	height = p->box.height();
	fore = colors::black;
	alpha = 128;
	strokeout(rectf, metrics::border + metrics::padding);
	alpha = push_alpha;
	fore = push_fore;
	textf(p->format);
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

static void paint_float_text() {
	pushrect push;
	for(auto& e : bsdata<floattext>()) {
		if(!e)
			continue;
		if(e.delay <= 0) {
			e.clear();
			continue;
		}
		e.delay -= current_tick_delta;
		if(!e.box.intersect(last_area))
			continue;
		caret = e.position - camera;
		caret.x += last_screen.x1;
		caret.y += last_screen.y1;
		paint_float_text(&e);
	}
	update_floattext_tail();
}

static int priority_normal(const drawable* object) {
	return object->position.y;
}

static int priority_ground(const drawable* object) {
	auto p = (itemground*)object;
	return object->position.y - 1000 - p->geti().ground;
}

static int priority_door(const drawable* object) {
	auto p = (door*)object;
	return p->getrect().y2 - 8;
}

static int priority_region(const drawable* object) {
	auto p = (door*)object;
	return p->getrect().y2 - 1000 - 8;
}

static int priority_animate(const drawable* object) {
	auto p = (animation*)object;
	return object->position.y + p->height;
}

static int compare_drawobject(const void* v1, const void* v2) {
	auto p1 = (drawobject*)v1;
	auto p2 = (drawobject*)v2;
	auto n1 = p1->render->priority(p1->object);
	auto n2 = p2->render->priority(p2->object);
	if(n1 != n2)
		return n1 - n2;
	if(p1->object->position.x != p1->object->position.y)
		return p1->object->position.x - p2->object->position.x;
	return p1->object - p2->object;
}

static void prepare_creatures() {
	update_creatures();
	auto pm = bsdata<renderi>::find("Creature");
	if(!pm)
		return;
	for(auto p : creatures) {
		if(!p->position.in(clipped_area))
			continue;
		if(!p->getrect().intersect(last_area))
			continue;
		add_object(pm, p);
	}
}

static void paint_objects() {
	pushrect push;
	clipped_area = last_area;
	clipped_area.offset(-128);
	update_floattext_tail();
	objects.clear();
	for(auto& e : bsdata<renderi>())
		add_objects(&e);
	prepare_creatures();
	objects.sort(compare_drawobject);
	for(auto& e : objects) {
		last_object = e.object;
		caret = last_object->position - camera;
		caret.x += last_screen.x1;
		caret.y += last_screen.y1;
		e.render->paint();
	}
}

static unsigned get_game_tick() {
	return current_tick / 64;
}

static void correct_camera() {
	if(camera.x + last_screen.width() > area_width * 16)
		camera.x = area_width * 16 - last_screen.width();
	if(camera.y + last_screen.height() > area_height_tiles * 16)
		camera.y = area_height_tiles * 16 - last_screen.height();
	if(camera.x < 0)
		camera.x = 0;
	if(camera.y < 0)
		camera.y = 0;
}

void setcamera(point v) {
	if(!last_screen.width())
		last_screen.set(0, 0, 800, 433);
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
	auto tx0 = camera.x / tile_size;
	auto ty0 = camera.y / tile_size;
	auto tdx = width / tile_size + 1;
	auto tdy = height / tile_size + 1;
	auto tx1 = tx0 + tdx;
	auto ty1 = ty0 + tdy;
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
	auto tx0 = camera.x / 16, ty0 = camera.y / 12;
	auto tx1 = tx0 + width / 16 + 1, ty1 = ty0 + height / 12 + 1;
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
	// auto blocked = is_block(index);
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
	if(hot.mouse.in(last_screen)) {
		if(!combat_mode || area_cost[s2i(hotspot)] < Blocked)
			cursor.set(pma_cursors, 4);
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
	cursor.set(pma_cursarw, index);
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

static void set_visible_area() {
	cursor.set(pma_cursors, 0);
	last_screen.set(caret.x, caret.y, caret.x + width, caret.y + height);
	auto push_caret = caret;
	auto mx = (area_width / 4) * tile_size;
	auto my = (area_height_tiles / 4) * tile_size;
	if(mx > width)
		mx = width;
	if(my > height)
		my = height;
	if((mx < width) || (my < height)) {
		rectblack({push_caret.x, push_caret.y, caret.x, push_caret.y + height});
		rectblack({caret.x + mx, push_caret.y, caret.x + width, push_caret.y + height});
		rectblack({push_caret.x, push_caret.y, caret.x + width, caret.y});
		rectblack({push_caret.x, caret.y + my, caret.x + width, caret.y + height});
	}
	last_area.set(caret.x, caret.y, caret.x + mx, caret.y + my);
	last_area.move(camera.x, camera.y);
	if(hot.mouse.in(last_screen))
		hotspot = hot.mouse - caret + camera;
	else
		hotspot = {-1000, -1000};
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

static void fill_polygon(const sliceu<point>& source) {
	auto pb = source.begin();
	auto pe = source.end();
	if(pb >= pe)
		return;
	pushrect push;
	auto y2 = caret.y + height;
	auto py1 = pb[0].y - camera.y;
	for(; caret.y < y2; caret.y++) {
		auto above = caret.y >= py1;
		auto outside = true;
		for(auto p = pb; p < pe; p++) {
			point p0 = p[0] - camera;
			point p1 = (((p + 1) == pe) ? *pb : p[1]) - camera;
			if(p0.y == p1.y && caret.y == p1.y) {
				caret.x = p0.x;
				line(p1.x, caret.y);
				continue;
			}
			auto cur_above = caret.y >= p1.y;
			if(above == cur_above)
				continue;
			auto dx = p0.x - p1.x;
			auto dy = p0.y - p1.y;
			auto dt = caret.y - p1.y;
			if(outside) {
				caret.x = p1.x + dt * dx / dy;
				outside = false;
			} else {
				line(p1.x, caret.y);
				outside = true;
			}
			above = cur_above;
		}
	}
}

static void polygon_green(const sliceu<point>& source) {
	auto push_fore = fore;
	fore = colors::green;
	polygon(source);
	fore = push_fore;
}

static void polygon_green_filled(const sliceu<point>& source, const rect& rc) {
	auto push_fore = fore;
	auto push_alpha = alpha; alpha = 64;
	pushrect push;
	caret.x = rc.x1 - camera.x;
	caret.y = rc.y1 - camera.y;
	width = rc.width();
	height = rc.height();
	fore = colors::green;
	fill_polygon(source);
	alpha = push_alpha;
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

static void paint_ground() {
	pushvalue push(palt, pallette);
	auto p = (itemground*)last_object;
	auto n = p->geti().ground;
	auto& f = pma_ground->get(n);
	if(hot.mouse.in(f.getrect(caret.x, caret.y, 0)))
		hilite_object = last_object;
	if(f.pallette) {
		memcpy(pallette, pma_ground->ptr(f.pallette), sizeof(pallette));
		if(hilite_object != last_object)
			pallette[1] = color(64, 64, 64);
	}
	image(pma_ground, n, ImagePallette);
}

static void paint_creature() {
	auto p = (creature*)last_object;
	if(hotspot.in(p->getbox()))
		hilite_object = last_object;
	if(p->ishilite())
		cursor.cicle = 0;
	paint_markers(p);
	p->paint();
}

static void paint_animation() {
	auto p = (animation*)last_object;
	auto pr = gres(p->rsname);
	if(!pr)
		return;
	auto hour = gethour();
	if(p->is(RenderBlackAsTransparent)) {
		auto push_alpha = alpha; alpha = alpha >> 2;
		image(pr, pr->ganim(p->frame, get_game_tick()), p->is(Mirrored) ? ImageMirrorV : 0);
		alpha = push_alpha;
	} else
		image(pr, pr->ganim(p->frame, get_game_tick()), p->is(Mirrored) ? ImageMirrorV : 0);
}

static void paint_door() {
	auto p = (door*)last_object;
	if(hotspot.in(p->box)) {
		auto n = p->getpoints();
		if(inside(hotspot, n.begin(), n.size()))
			hilite_object = last_object;
	}
	if(p->ishilite()) {
		polygon_green_filled(p->getpoints(), p->box);
		polygon_green(p->getpoints());
		cursor.cicle = p->cursor;
	}
}

static void paint_region() {
	auto p = (region*)last_object;
	if(hotspot.in(p->box) && inside(hotspot, p->points.begin(), p->points.size()))
		hilite_object = last_object;
	if(p->ishilite()) {
		switch(p->type) {
		case RegionInfo: cursor.cicle = 22; break;
		case RegionTravel: cursor.cicle = 34; break;
		}
	}
}

static void paint_container() {
	auto p = (container*)last_object;
	if(hotspot.in(p->box) && inside(hotspot, p->points.begin(), p->points.size()))
		hilite_object = last_object;
	if(p->ishilite()) {
		polygon_green_filled(p->points, p->box);
		polygon_green(p->points);
		cursor.cicle = 2;
	}
}

static const char* gettipsname(point position) {
	return str("%3Info%1i_%2i", position.x, position.y, area_name);
}

static void apply_hilite_command() {
	if(!hilite_object)
		return;
	if(hot.key == MouseLeft && !hot.pressed) {
		if(bsdata<region>::have(hilite_object)) {
			auto p = (region*)(drawable*)hilite_object;
			if(p->type == RegionInfo) {
				auto pn = getnme(gettipsname(p->position));
				if(pn) {
					add_float_text(hotspot, pn, 320, 1000 * 5, p);
					print("[+%1]", pn);
				}
			} else if(p->type == RegionTravel)
				enter(p->move_to_entrance);
		} else if(bsdata<door>::have(hilite_object)) {
			auto p = (door*)(drawable*)hilite_object;
			p->use(!p->isopen());
		} else if(bsdata<container>::have(hilite_object)) {
			auto p = (container*)(drawable*)hilite_object;
			print("This is container %1i", getbsi(p));
			party_action(p, p->launch, open_container);
		} else if(bsdata<creature>::have(hilite_object)) {
			if(combat_mode) {

			} else
				execute(choose_creature, 0, 0, (drawable*)hilite_object);
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
	if(hilite_object)
		return;
	if(hot.mouse.in(last_screen)) {
		if(hot.pressed) {
			auto rc = create_rect(left_mouse_press + camera, hot.mouse + camera);
			if(rc.size() >= 8) {
				pushrect push;
				pushfore push_fore(colors::green);
				cursor.clear();
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
	if(hilite_object)
		return;
	if(!hot.mouse.in(last_screen))
		return;
	auto map_index = s2i(hotspot);
	if(area_cost[map_index] >= Blocked) {
		cursor.set(pma_cursors, 6);
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
	set_visible_area();
	set_standart_cursor();
	paint_tiles();
	paint_movement_target();
#ifdef _DEBUG
	paint_block_area();
#endif // _DEBUG
	paint_objects();
	paint_float_text();
	apply_hilite_command();
	if(combat_mode)
		apply_command_combat();
	else
		apply_command();
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

void paint_area() {
	paint_area_map_zoomed(paint_area_map);
	apply_shifer();
	apply_keybinding();
	update_orders();
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
	pushrect push; setcaret(0, 0, 800, 600);
	auto push_clip = clipping; setclip();
	set_visible_area();
	paint_tiles();
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
		enter_from_wmap(p->id);
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

static int container_frame(container::typen type) {
	switch(type) {
	case container::Bag: return 1;
	case container::Barrel: return 3;
	case container::Altar: return 0;
	case container::Body: return 4;
	case container::Spellbook: return 5;
	default: return 0;
	}
}

static void paint_pick_container() {
	auto pc = gres("CONTAINER");
	paint_game_dialog(0, 476, "GUICONT", 1);
	setdialog(62, 25); image(pc, container_frame(last_container->type), 0);
	setdialog(430, 28); image(pc, 1, 0);
	setdialog(150, 22); stoneslot(0, 0);
	setdialog(195, 22); stoneslot(0, 0);
	setdialog(239, 22); stoneslot(0, 0);
	setdialog(283, 22); stoneslot(0, 0);
	setdialog(327, 22); stoneslot(0, 0);
	setdialog(150, 65); stoneslot(0, 0);
	setdialog(195, 65); stoneslot(0, 1);
	setdialog(239, 65); stoneslot(0, 1);
	setdialog(283, 65); stoneslot(0, 1);
	setdialog(327, 65); stoneslot(0, 1);
	//Scroll GBTNSCRL 375 24 12 76 frames(1 0 3 2 4 5)
	setdialog(509, 22); stoneslot(0, 0);
	setdialog(553, 22); stoneslot(0, 0);
	setdialog(553, 65); stoneslot(0, 0);
	setdialog(509, 65); stoneslot(0, 0);
	//Scroll GBTNSCRL 602 24 12 76 frames(1 0 3 2 4 5)
	setdialog(661, 78, 70, 20); texta(str("%1i", player->coins), AlignRightCenter);
	setdialog(684, 28); button(pma_butopt1, 1, 2, KeyEscape); fire(buttoncancel);
}

static void mouse_area_cancel() {
	if(hot.key == MouseLeft && !hot.pressed && ishilite())
		breakmodal(0);
}

static void paint_container_area() {
	update_frames();
	setcaret(0, 0, 800, 476);
	mouse_area_cancel();
	paint_area_map_zoomed(paint_area_map);
	apply_shifer();
	paint_pick_container();
}

void open_container() {
	pushvalue push_container(last_container);
	last_container = (container*)hot.object;
	if(!last_container)
		return;
	scene(paint_container_area);
}

BSDATA(renderi) = {
	{"Animation", bsdata<animation>::source, bsdata<animation>::elements, paint_animation, allow_animate, priority_animate},
	{"Container", bsdata<container>::source, bsdata<container>::elements, paint_container, allow_clipped_area, priority_normal},
	{"Creature", bsdata<creature>::source, bsdata<creature>::elements, paint_creature, 0, priority_normal},
	{"Door", bsdata<door>::source, bsdata<door>::elements, paint_door, allow_door, priority_door},
	{"Item", bsdata<itemground>::source, bsdata<itemground>::elements, paint_ground, allow_ground, priority_ground},
	{"Region", bsdata<region>::source, bsdata<region>::elements, paint_region, allow_region, priority_region},
};
BSDATAF(renderi)