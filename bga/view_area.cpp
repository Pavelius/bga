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
#include "map.h"
#include "math.h"
#include "region.h"
#include "resid.h"
#include "resinfo.h"
#include "timer.h"
#include "view.h"
#include "worldmap.h"

using namespace draw;

static worldmapi::area* current_world_area_hilite;

void paperdoll(color* pallette, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm);
void scale2x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned width, unsigned height);

const int tile_size = 64;

static point hotspot;
static int zoom_factor = 1;

static unsigned get_game_tick() {
	return current_tick / 64;
}

static void correct_camera() {
	if(camera.x + last_screen.width() > map::width * 16)
		camera.x = map::width * 16 - last_screen.width();
	if(camera.x < 0)
		camera.x = 0;
	if(camera.y + last_screen.height() > map::height_tiles * 16)
		camera.y = map::height_tiles * 16 - last_screen.height();
	if(camera.y < 0)
		camera.y = 0;
}

void setcamera(point v) {
	v.x -= last_screen.width() / 2;
	v.y -= last_screen.height() / 2;
	camera = v;
	correct_camera();
}

static void actor_marker(int size, bool flicking, bool double_border) {
	auto r = (size + 1) * 6;
	if(flicking)
		r += iabs(int(get_game_tick() % 6) - 3) - 1;
	circle(r);
	if(double_border)
		circle(r + 1);
}

static void paint_tiles() {
	auto sp = map::getareasprite();
	if(!sp)
		return;
	int tx0 = camera.x / tile_size, ty0 = camera.y / tile_size;
	int dx = width / tile_size + 1, dy = height / tile_size + 1;
	int tx1 = tx0 + dx, ty1 = ty0 + dy;
	if(tx1 > map::width / 4 - 1)
		tx1 = map::width / 4 - 1;
	if(ty1 > map::height_tiles / 4 - 1)
		ty1 = map::height_tiles / 4 - 1;
	int ty = ty0;
	while(ty <= ty1) {
		int tx = tx0;
		while(tx <= tx1) {
			auto x = tx * tile_size - camera.x;
			auto y = ty * tile_size - camera.y;
			draw::image(x, y, sp, map::gettile(ty * 64 + tx), 0);
			tx++;
		}
		ty++;
	}
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
	if(!hot.mouse.in(last_screen))
		cursor.set(CURSORS, 0);
	else
		cursor.set(CURSORS, 4);
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

static void setup_visible_area() {
	last_screen.set(caret.x, caret.y, caret.x + width, caret.y + height);
	last_area = last_screen; last_area.move(camera.x, camera.y);
	last_area.offset(-128, -128);
	hotspot = camera + hot.mouse;
	hilite_drawable = 0;
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
	fore = colors::green;
	if(p->isselected())
		actor_marker(1, false, player == p);
	fore = push_fore;
}

void creature::paint() const {
	paint_markers(this);
	actor::paint();
	//color pallette[256]; setpallette(pallette);
	//apply_shadow(pallette, map::getshadow(position));
	//paperdoll(pallette,
	//	race, gender, getmainclass(), 1, orientation, get_game_tick(),
	//	wears[Body], getweapon(), getoffhand(), wears[Head]);
}

void animation::paint() const {
	auto pr = gres(this->rsname, "art/animations");
	if(!pr)
		return;
	auto hour = gethour();
	if(is(RenderBlackAsTransparent)) {
		//image_tint(caret.x, caret.y, pr, pr->ganim(frame, get_game_tick()), is(Mirrored) ? ImageMirrorV : 0);
		auto push_alpha = alpha;
		alpha = alpha >> 2;
		image(pr, pr->ganim(frame, get_game_tick()), is(Mirrored) ? ImageMirrorV : 0);
		alpha = push_alpha;
	} else
		image(pr, pr->ganim(frame, get_game_tick()), is(Mirrored) ? ImageMirrorV : 0);
}

static void paint_object(drawable* object) {
	if(bsdata<door>::have(object)) {
		auto p = (door*)object;
		if(p->ishilite()) {
			polygon_green(p->getpoints());
			cursor.cicle = p->cursor;
		}
	} else if(bsdata<region>::have(object)) {
		auto p = (region*)object;
		if(p->ishilite()) {
			switch(p->type) {
			case RegionInfo: cursor.cicle = 22; break;
			case RegionTravel: cursor.cicle = 34; break;
			}
		}
	} else if(bsdata<container>::have(object)) {
		auto p = (container*)object;
		if(p->ishilite()) {
			polygon_green(p->points);
			cursor.cicle = 2;
		}
	} else if(bsdata<floattext>::have(object)) {
		auto p = (floattext*)object;
		p->paint();
	} else if(bsdata<creature>::have(object)) {
		auto p = (creature*)object;
		p->paint();
	} else if(bsdata<animation>::have(object)) {
		auto p = (animation*)object;
		p->paint();
	}
}

static bool ishilite(const drawable* object) {
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
	}
	return false;
}

static void paint_objects() {
	auto push_caret = caret;
	for(auto p : objects) {
		caret = p->position - camera;
		if(ishilite(p))
			hilite_drawable = p;
		paint_object(p);
	}
}

static const char* gettipsname(point position) {
	return str("%3Info%1i_%2i", position.x, position.y, map::areaname);
}

static void apply_hilite_command() {
	if(!hilite_drawable)
		return;
	if(hot.key == MouseLeft && hot.pressed) {
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
			print("This is %1", p->name);
			player->moveto(p->launch);
		}
	}
}

static void jump_party() {
	setparty(hotspot);
}

static void apply_command() {
	if(hilite_drawable)
		return;
	if(hot.key == MouseLeft && hot.pressed && hot.mouse.in(last_screen))
		execute(jump_party);
}

static void paint_area_map() {
	auto push_clip = clipping; setclipall();
	setup_visible_area();
	set_standart_cursor();
	paint_tiles();
	prepare_objects();
	sort_objects();
	paint_objects();
	apply_hilite_command();
	apply_command();
	clipping = push_clip;
}

static void paint_area_map_zoom_factor() {
	auto push_clipping = clipping;
	auto push_mouse = hot.mouse; hot.mouse.x /= zoom_factor; hot.mouse.y /= zoom_factor;
	pushrect push; width /= zoom_factor; height /= zoom_factor;
	static surface temporary_canvas; temporary_canvas.resize(width, height, 32, true);
	auto push_canvas = canvas;
	canvas = &temporary_canvas; setclip();
	paint_area_map();
	canvas = push_canvas;
	if(zoom_factor == 2) {
		scale2x(canvas->ptr(push.caret.x, push.caret.y), canvas->scanline,
			temporary_canvas.ptr(0, 0), temporary_canvas.scanline,
			width, height);
	}
	hot.mouse = push_mouse;
	clipping = push_clipping;
}

static void paint_area_map_zoomed() {
	if(zoom_factor <= 1)
		paint_area_map();
	else
		paint_area_map_zoom_factor();
}

void paint_area() {
	paint_area_map_zoomed();
	apply_shifer();
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
	auto mm = map::getminimap();
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