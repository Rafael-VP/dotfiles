/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h> /* Special function keys */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static void tile(Monitor *m);
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10", "Symbola:pixelsize=10:antialias=true:autohint=true:fixed=true" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spfm", "-g", "120x34", "-e", "ranger", NULL };
const char *spcmd3[] = {"st", "-n", "sptm", "-g", "120x34", "-e", "htop", NULL };
const char *spcmd4[] = {"st", "-n", "spam", "-g", "120x34", "-e", "alsamixer", NULL };
const char *spcmd5[] = {"st", "-n", "sppm", "-g", "120x34", "-e", "pulsemixer", NULL };
const char *spcmd6[] = {"st", "-n", "spcl", "-g", "120x34", "-e", "wcalc", NULL };
const char *spcmd7[] = {"st", "-n", "spmp", "-g", "120x34", "-e", "ncmpcpp", NULL };
const char *spcmd8[] = {"st", "-n", "spnp", "-g", "120x34", "-e", "nvim", NULL };

static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spfm", 	spcmd2},
	{"sptm",    	spcmd3},
	{"spam", 	spcmd4},
	{"sppm", 	spcmd5},
	{"spcl", 	spcmd6},
	{"spmp", 	spcmd7},
	{"spnp", 	spcmd8},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      inwstance    title       tags mask     isfloating   monitor */
	{ NULL,	      "spterm",	  NULL,	      SPTAG(0),     1,		 -1 },
	{ NULL,	      "spfm",     NULL,	      SPTAG(1),	    1,		 -1 },
	{ NULL,	      "sptm",	  NULL,	      SPTAG(2),	    1,		 -1 },
	{ NULL,	      "spam",	  NULL,	      SPTAG(3),	    1,		 -1 },
	{ NULL,	      "sppm",	  NULL,	      SPTAG(4),	    1,		 -1 },
	{ NULL,	      "spcl",	  NULL,	      SPTAG(5),	    1,		 -1 },
	{ NULL,	      "spmp",	  NULL,	      SPTAG(6),	    1,		 -1 },
	{ NULL,	      "spnp",	  NULL,	      SPTAG(7),	    1,		 -1 },
 };

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-i", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key        					function        		argument */
	{ 0,       			XF86XK_MonBrightnessUp, 			spawn, 				SHCMD("brightness +") },
	{ 0,       			XF86XK_MonBrightnessDown, 			spawn, 				SHCMD("brightness -") },
	{ 0,       			XF86XK_AudioLowerVolume, 			spawn, 				SHCMD("amixer sset 'Master' 5%-") },
	{ 0,       			XF86XK_AudioRaiseVolume, 			spawn, 				SHCMD("amixer sset 'Master' 5%+") },
	{ 0,       			XF86XK_AudioMute, 				spawn, 				SHCMD("amixer sset Master toggle & amixer sset Headphone toggle") },
	{ 0,       			XF86XK_AudioPlay, 				spawn, 				SHCMD("xdotool key XF86AudioPlay") },
	{ 0,       			XF86XK_AudioPrev, 				spawn, 				SHCMD("xdotool key XF86AudioPrev") },
	{ 0,       			XF86XK_AudioNext, 				spawn, 				SHCMD("xdotool key XF86AudioNext") },
	{ 0,       			XK_Print, 					spawn, 				SHCMD("screenshot") },
	{ ShiftMask,       		XK_Print, 					spawn, 				SHCMD("screenshot select") },
	{ MODKEY|ShiftMask,       	XK_Print, 					spawn, 				SHCMD("screenshot window") },
	{ MODKEY,            		XK_c,  	   					togglescratch,  		{.ui = 0 } },
	{ MODKEY,            		XK_f,	   					togglescratch,  		{.ui = 1 } },
	{ MODKEY,            		XK_r,	   					togglescratch,  		{.ui = 2 } },
	{ MODKEY,            		XK_v,	   					togglescratch,  		{.ui = 3 } },
	{ MODKEY|ShiftMask,            	XK_v,	   					togglescratch,  		{.ui = 4 } },
	{ MODKEY|ShiftMask,            	XK_c,	   					togglescratch,  		{.ui = 5 } },
	{ MODKEY,            		XK_m,	   					togglescratch,  		{.ui = 6 } },
	{ MODKEY,            		XK_n,	   					togglescratch,  		{.ui = 7 } },
	{ MODKEY,            		XK_w,	   					spawn,				SHCMD("firefox") },
	{ MODKEY,            		XK_d,	   					spawn,				SHCMD("discord") },
	{ Mod1Mask,            		XK_x,	   					spawn,				SHCMD("pasuspender mpc toggle") },
	{ Mod1Mask,            		XK_z,	   					spawn,				SHCMD("pasuspender mpc prev") },
	{ Mod1Mask,            		XK_c,	   					spawn,				SHCMD("pasuspender mpc next") },
	{ Mod1Mask,            		XK_v,	   					spawn,				SHCMD("pasuspender mpcshufflealbum") },
	{ MODKEY|ShiftMask,           	XK_m,	   					spawn,				SHCMD("mpcsearch") },
	{ MODKEY,           		XK_a,	   					spawn,				SHCMD("mpdswitch") },
	{ MODKEY,                       XK_p,      					spawn,          		{.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, 					spawn,          		{.v = termcmd } },
	{ MODKEY,                       XK_b,      					togglebar,      		{0} },
	{ MODKEY,                       XK_j,      					focusstack,     		{.i = +1 } },
	{ MODKEY,                       XK_k,      					focusstack,     		{.i = -1 } },
	{ MODKEY,                       XK_i,      					incnmaster,     		{.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      					incnmaster,     		{.i = -1 } },
	{ MODKEY,                       XK_h,      					setmfact,       		{.f = -0.05} },
	{ MODKEY,                       XK_l,      					setmfact,       		{.f = +0.05} },
	{ MODKEY,                       XK_Return, 					zoom,           		{0} },
	{ MODKEY,              		XK_u,      					incrgaps,       		{.i = +1 } },
	{ MODKEY|ShiftMask,    		XK_u,      					incrgaps,       		{.i = -1 } },
	{ MODKEY,              		XK_o,      					togglegaps,     		{0} },
	{ MODKEY|ShiftMask,    		XK_o,      					defaultgaps,    		{0} },
	{ MODKEY,                       XK_Tab,    					view,           		{0} },
	{ MODKEY,             		XK_q,      					killclient,     		{0} },
	{ MODKEY,                       XK_t,      					setlayout,      		{.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_r,      					setlayout,      		{.v = &layouts[8]} },
	{ MODKEY,                       XK_space,  					setlayout,      		{0} },
	{ MODKEY|ShiftMask,             XK_space,  					togglefloating, 		{0} },
	{ MODKEY,                       XK_0,      					view,           		{.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      					tag,            		{.ui = ~0 } },
	{ MODKEY,                       XK_comma,  					focusmon,       		{.i = -1 } },
	{ MODKEY,                       XK_period, 					focusmon,       		{.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  					tagmon,         		{.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, 					tagmon,         		{.i = +1 } },
	TAGKEYS(                        XK_1,      					                		0)
	TAGKEYS(                        XK_2,      					                		1)
	TAGKEYS(                        XK_3,      					                		2)
	TAGKEYS(                        XK_4,      					                		3)
	TAGKEYS(                        XK_5,      					                		4)
	TAGKEYS(                        XK_6,      					                		5)
	TAGKEYS(                        XK_7,      					                		6)
	TAGKEYS(                        XK_8,      					                		7)
	TAGKEYS(                        XK_9,      					                		8)
	{ MODKEY|ShiftMask,             XK_q,      					quit,           		{0} },
};		

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
}
