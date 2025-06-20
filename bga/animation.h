#include "drawable.h"
#include "variable.h"

#pragma once

struct sprite;

enum animatef_s : unsigned char {
	AnimationEnabled, RenderBlackAsTransparent, NonSelfIllumination, PartialAnimation,
	SynchronizedDraw, UseRandomStartFrame, WallDoesntHideAnimation, DisableOnSlowMachines,
	NotCover, PlayAllFrames, UsePalatteBitmap, Mirrored,
	ShowInCombat
};

struct animation : drawable, variableid {
	unsigned			shedule;
	char				rsname[9], rsname_pallette[9];
	unsigned short		circle, frame, start_frame;
	unsigned			flags;
	unsigned short		height;
	unsigned char		transparency;
	unsigned char		chance_loop;
	unsigned char		skip_cycles;
	bool				isvisible() const;
	bool				is(animatef_s v) const { return (flags & (1 << v)) != 0; }
};
