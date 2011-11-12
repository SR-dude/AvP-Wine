# Makefile - AvP.dsp

ifndef CFG
CFG=AvP - Win32 Debug
endif
CC=winegcc
CFLAGS=
CXX=wineg++
CXXFLAGS=$(CFLAGS)
RC=windres -O COFF
ifeq "$(CFG)"  "AvP - Win32 Release"
CFLAGS+=-I/usr/include/wine/msvcrt -I/usr/include/wine/windows -DWIN32 -D_WINDOWS -w  -fexceptions -fpermissive -O2 -I3dc -I3dc/avp -Dengine=1 -I3dc/avp/support -I3dc/avp/win95 -DAVP_DEBUG_VERSION -I3dc/avp/win95/frontend -I3dc/avp/win95/gadgets -I3dc/include -I3dc/win95
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
TARGET=avpprog.exe
LDFLAGS+=-L3dc -Wl,--subsystem,windows
LIBS+=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lddraw -ldsound -ldplayx -ldinput -lsmackw32 -lbinkw32 -lwinmm
else
ifeq "$(CFG)"  "AvP - Win32 Debug"
CFLAGS+=-I/usr/include/wine/msvcrt -I/usr/include/wine/windows -I3dc/include -I3dc/win95 -w -DWIN32 -fexceptions -fpermissive -march=i686 -g -O0 -D_DEBUG -I3dc -D_WINDOWS -I3dc/avp  -I3dc/avp/support -Dengine=1 -I3dc/avp/win95 -I3dc/avp/win95/frontend -DAVP_DEBUG_VERSION -I3dc/avp/win95/gadgets
LD=$(CXX)
LDFLAGS=-m32
TARGET=debug_AvP.exe
LDFLAGS+=
LIBS+=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lddraw -ldsound -ldplayx -ldinput -lwinmm -lmsvcrt
#LIBS+=
else
ifeq "$(CFG)"  "AvP - Win32 Release For Fox"
CFLAGS+=-I/usr/include/wine/msvcrt -I/usr/include/wine/windows -DWIN32 -D_WINDOWS -w -fexceptions -fpermissive -O2 -I3dc -I3dc/avp -Dengine=1 -I3dc/avp/support -I3dc/avp/win95 -I3dc/avp/win95/frontend -I3dc/avp/win95/gadgets -I3dc/include -I3dc/win95
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
TARGET=AvP.exe
LDFLAGS+=-L3dc -Wl,--subsystem,windows
LIBS+=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lddraw -ldsound -ldplayx -ldinput -lsmackw32 -lbinkw32 -lwinmm
endif
endif
endif

ifndef TARGET
TARGET=AvP.exe
endif

.PHONY: all
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

%.res: %.rc
	$(RC) $(CPPFLAGS) -o $@ -i $<

SOURCE_FILES= \
	3dc/afont.c \
	3dc/avp/win95/gadgets/ahudgadg.cpp \
	3dc/avp/ai_sight.c \
	3dc/win95/alt_tab.cpp \
	3dc/win95/animchnk.cpp \
	3dc/win95/animobs.cpp \
	3dc/avp/win95/frontend/avp_envinfo.c \
	3dc/avp/win95/frontend/avp_intro.cpp \
	3dc/avp/win95/frontend/avp_menudata.c \
	3dc/avp/win95/frontend/avp_menugfx.cpp \
	3dc/avp/win95/frontend/avp_menus.c \
	3dc/avp/win95/frontend/avp_mp_config.cpp \
	3dc/avp/win95/frontend/avp_userprofile.cpp \
	3dc/avp/win95/avpchunk.cpp \
	3dc/avp/win95/avpreg.cpp \
	3dc/avp/avpview.c \
	3dc/win95/awbmpld.cpp \
	3dc/win95/awiffld.cpp \
	3dc/win95/awpnmld.cpp \
	3dc/win95/awtexld.cpp \
	3dc/avp/bh_agun.c \
	3dc/avp/bh_ais.c \
	3dc/avp/bh_alien.c \
	3dc/avp/bh_binsw.c \
	3dc/avp/bh_cable.c \
	3dc/avp/bh_corpse.c \
	3dc/avp/bh_deathvol.c \
	3dc/avp/bh_debri.c \
	3dc/avp/bh_dummy.c \
	3dc/avp/bh_fan.c \
	3dc/avp/bh_far.c \
	3dc/avp/bh_fhug.c \
	3dc/avp/bh_gener.c \
	3dc/avp/bh_ldoor.c \
	3dc/avp/bh_lift.c \
	3dc/avp/bh_light.c \
	3dc/avp/bh_lnksw.c \
	3dc/avp/bh_ltfx.c \
	3dc/avp/bh_marin.c \
	3dc/avp/bh_mission.c \
	3dc/avp/bh_near.c \
	3dc/avp/bh_pargen.c \
	3dc/avp/bh_plachier.c \
	3dc/avp/bh_plift.c \
	3dc/avp/bh_pred.c \
	3dc/avp/bh_queen.c \
	3dc/avp/bh_rubberduck.c \
	3dc/avp/bh_selfdest.c \
	3dc/avp/bh_snds.c \
	3dc/avp/bh_spcl.c \
	3dc/avp/bh_swdor.c \
	3dc/avp/bh_track.c \
	3dc/avp/bh_types.c \
	3dc/avp/bh_videoscreen.c \
	3dc/avp/bh_waypt.c \
	3dc/avp/bh_weap.c \
	3dc/avp/bh_xeno.c \
	3dc/win95/bink.c \
	3dc/win95/bmpnames.cpp \
	3dc/avp/bonusabilities.c \
	3dc/avp/cconvars.cpp \
	3dc/win95/cd_player.c \
	3dc/avp/cdtrackselection.cpp \
	3dc/avp/win95/cheat.c \
	3dc/avp/cheatmodes.c \
	3dc/win95/chnkload.cpp \
	3dc/win95/chnktexi.cpp \
	3dc/win95/chnktype.cpp \
	3dc/avp/win95/chtcodes.cpp \
	3dc/win95/chunk.cpp \
	3dc/win95/chunkpal.cpp \
	3dc/avp/comp_map.c \
	3dc/avp/comp_shp.c \
	3dc/avp/support/consbind.cpp \
	3dc/avp/support/consbtch.cpp \
	3dc/avp/win95/gadgets/conscmnd.cpp \
	3dc/avp/consolelog.cpp \
	3dc/avp/win95/gadgets/conssym.cpp \
	3dc/avp/win95/gadgets/consvar.cpp \
	3dc/avp/support/coordstr.cpp \
	3dc/avp/shapes/cube.c \
	3dc/win95/d3_func.cpp \
	3dc/avp/win95/d3d_hud.cpp \
	3dc/avp/win95/d3d_render.cpp \
	3dc/avp/support/daemon.cpp \
	3dc/avp/davehook.cpp \
	3dc/win95/db.c \
	3dc/win95/dd_func.cpp \
	3dc/avp/win95/ddplat.cpp \
	3dc/avp/deaths.c \
	3dc/win95/debuglog.cpp \
	3dc/avp/decal.c \
	3dc/avp/detaillevels.c \
	3dc/avp/win95/directplay.c \
	3dc/avp/win95/dp_func.c \
	3dc/avp/win95/dplayext.c \
	3dc/win95/dummyobjectchunk.cpp \
	3dc/avp/win95/dx_proj.cpp \
	3dc/win95/dxlog.c \
	3dc/avp/dynamics.c \
	3dc/avp/dynblock.c \
	3dc/avp/win95/endianio.c \
	3dc/win95/enumchnk.cpp \
	3dc/win95/enumsch.cpp \
	3dc/win95/envchunk.cpp \
	3dc/avp/equipmnt.c \
	3dc/avp/equiputl.cpp \
	3dc/avp/extents.c \
	3dc/win95/fail.c \
	3dc/avp/win95/ffread.cpp \
	3dc/avp/win95/ffstdio.cpp \
	3dc/win95/fragchnk.cpp \
	3dc/frustrum.c \
	3dc/avp/win95/gadgets/gadget.cpp \
	3dc/avp/game.c \
	3dc/avp/game_statistics.c \
	3dc/avp/gamecmds.cpp \
	3dc/avp/gameflow.c \
	3dc/avp/gamevars.cpp \
	3dc/avp/win95/gammacontrol.cpp \
	3dc/avp/win95/gflwplat.c \
	3dc/win95/gsprchnk.cpp \
	3dc/win95/hierchnk.cpp \
	3dc/avp/win95/hierplace.cpp \
	3dc/avp/hmodel.c \
	3dc/avp/hud.c \
	3dc/avp/win95/gadgets/hudgadg.cpp \
	3dc/win95/huffman.cpp \
	3dc/win95/iff.cpp \
	3dc/win95/iff_ilbm.cpp \
	3dc/win95/ilbm_ext.cpp \
	3dc/avp/support/indexfnt.cpp \
	3dc/avp/win95/intro.cpp \
	3dc/avp/inventry.c \
	3dc/win95/io.c \
	3dc/avp/win95/iofocus.cpp \
	3dc/avp/win95/jsndsup.cpp \
	3dc/kshape.c \
	3dc/avp/win95/kzsort.c \
	3dc/avp/win95/langplat.c \
	3dc/avp/language.c \
	3dc/avp/lighting.c \
	3dc/win95/list_tem.cpp \
	3dc/avp/load_shp.c \
	3dc/avp/los.c \
	3dc/win95/ltchunk.cpp \
	3dc/map.c \
	3dc/avp/maps.c \
	3dc/maths.c \
	3dc/win95/md5.c \
	3dc/win95/media.cpp \
	3dc/mem3dc.c \
	3dc/mem3dcpp.cpp \
	3dc/avp/mempool.c \
	3dc/avp/messagehistory.c \
	3dc/win95/mishchnk.cpp \
	3dc/avp/missions.cpp \
	3dc/avp/win95/modcmds.cpp \
	3dc/module.c \
	3dc/morph.c \
	3dc/avp/win95/mousecentreing.cpp \
	3dc/avp/movement.c \
	3dc/avp/mp_launch.c \
	3dc/mslhand.c \
	3dc/avp/win95/npcsetup.cpp \
	3dc/win95/obchunk.cpp \
	3dc/object.c \
	3dc/avp/win95/objsetup.cpp \
	3dc/win95/oechunk.cpp \
	3dc/win95/our_mem.c \
	3dc/avp/paintball.c \
	3dc/avp/particle.c \
	3dc/avp/win95/pathchnk.cpp \
	3dc/avp/win95/pcmenus.cpp \
	3dc/avp/pfarlocs.c \
	3dc/avp/pheromon.c \
	3dc/win95/plat_shp.c \
	3dc/avp/win95/platsup.c \
	3dc/avp/player.c \
	3dc/avp/win95/pldghost.c \
	3dc/avp/win95/pldnet.c \
	3dc/win95/plspecfn.c \
	3dc/avp/pmove.c \
	3dc/avp/win95/progress_bar.cpp \
	3dc/avp/win95/projload.cpp \
	3dc/avp/psnd.c \
	3dc/avp/win95/psndplat.c \
	3dc/avp/psndproj.c \
	3dc/avp/pvisible.c \
	3dc/avp/support/r2base.cpp \
	3dc/avp/support/r2pos666.cpp \
	3dc/avp/support/reflist.cpp \
	3dc/avp/support/refobj.cpp \
	3dc/avp/support/rentrntq.cpp \
	3dc/avp/win95/gadgets/rootgadg.cpp \
	3dc/avp/savegame.c \
	3dc/avp/scream.cpp \
	3dc/avp/win95/scrshot.cpp \
	3dc/avp/support/scstring.cpp \
	3dc/avp/secstats.c \
	3dc/avp/sfx.c \
	3dc/shpanim.c \
	3dc/win95/shpchunk.cpp \
	3dc/win95/smacker.c \
	3dc/win95/sndchunk.cpp \
	3dc/sphere.c \
	3dc/win95/sprchunk.cpp \
	3dc/avp/win95/strachnk.cpp \
	3dc/avp/stratdef.c \
	3dc/win95/string.cpp \
	3dc/avp/support/strtab.cpp \
	3dc/avp/support/strutil.c \
	3dc/avp/win95/system.c \
	3dc/avp/win95/gadgets/t_ingadg.cpp \
	3dc/tables.c \
	3dc/avp/support/tallfont.cpp \
	3dc/avp/targeting.c \
	3dc/avp/win95/gadgets/teletype.cpp \
	3dc/win95/texio.c \
	3dc/avp/win95/gadgets/textexp.cpp \
	3dc/avp/win95/gadgets/textin.cpp \
	3dc/win95/toolchnk.cpp \
	3dc/avp/track.c \
	3dc/avp/win95/gadgets/trepgadg.cpp \
	3dc/avp/support/trig666.cpp \
	3dc/avp/triggers.c \
	3dc/win95/txioctrl.cpp \
	3dc/avp/win95/usr_io.c \
	3dc/vdb.c \
	3dc/version.c \
	3dc/win95/videomodes.cpp \
	3dc/avp/win95/vision.c \
	3dc/win95/vramtime.c \
	3dc/avp/weapons.c \
	3dc/win95/win_func.cpp \
	3dc/avp/win95/win_proj.cpp \
	3dc/avp/win95/winmain.c \
	3dc/win95/wpchunk.cpp \
	3dc/avp/support/wrapstr.cpp \
	3dc/win95/zsp.cpp \
	3dc/win95/di_func.cpp \
	3dc/mathline.c \
	stubs.c

HEADER_FILES= \
	3dc/win95/advwin32.h \
	3dc/avp/win95/gadgets/ahudgadg.hpp \
	3dc/avp/ai_sight.h \
	3dc/win95/alt_tab.h \
	3dc/win95/animchnk.hpp \
	3dc/win95/animobs.hpp \
	3dc/avp/win95/frontend/avp_envinfo.h \
	3dc/avp/win95/frontend/avp_menugfx.hpp \
	3dc/avp/win95/frontend/avp_menus.h \
	3dc/avp/win95/frontend/avp_mp_config.h \
	3dc/avp/win95/frontend/avp_userprofile.h \
	3dc/avp/win95/avpchunk.hpp \
	3dc/avp/avpitems.hpp \
	3dc/avp/avppages.hpp \
	3dc/avp/win95/avpreg.hpp \
	3dc/avp/avpview.h \
	3dc/win95/aw.h \
	3dc/win95/awtexld.h \
	3dc/win95/awtexld.hpp \
	3dc/avp/bh_agun.h \
	3dc/avp/bh_ais.h \
	3dc/avp/bh_alien.h \
	3dc/avp/bh_binsw.h \
	3dc/avp/bh_cable.h \
	3dc/avp/bh_corpse.h \
	3dc/avp/bh_deathvol.h \
	3dc/avp/bh_debri.h \
	3dc/avp/bh_dummy.h \
	3dc/avp/bh_fan.h \
	3dc/avp/bh_far.h \
	3dc/avp/bh_fhug.h \
	3dc/avp/bh_gener.h \
	3dc/avp/bh_ldoor.h \
	3dc/avp/bh_lift.h \
	3dc/avp/bh_light.h \
	3dc/avp/bh_lnksw.h \
	3dc/avp/bh_ltfx.h \
	3dc/avp/bh_marin.h \
	3dc/avp/bh_mission.h \
	3dc/avp/bh_near.h \
	3dc/avp/bh_paq.h \
	3dc/avp/bh_pargen.h \
	3dc/avp/bh_plachier.h \
	3dc/avp/bh_plift.h \
	3dc/avp/bh_pred.h \
	3dc/avp/bh_queen.h \
	3dc/avp/bh_rubberduck.h \
	3dc/avp/bh_selfdest.h \
	3dc/avp/bh_snds.h \
	3dc/avp/bh_spcl.h \
	3dc/avp/bh_swdor.h \
	3dc/avp/bh_track.h \
	3dc/avp/bh_types.h \
	3dc/avp/bh_videoscreen.h \
	3dc/avp/bh_waypt.h \
	3dc/avp/bh_weap.h \
	3dc/avp/bh_xeno.h \
	3dc/win95/bink.h \
	3dc/win95/bink_rad.h \
	3dc/avp/win95/bmp2.h \
	3dc/win95/bmpnames.hpp \
	3dc/avp/bonusabilities.h \
	3dc/win95/cd_player.h \
	3dc/avp/cdtrackselection.h \
	3dc/avp/win95/cheat.h \
	3dc/win95/chnkload.h \
	3dc/win95/chnkload.hpp \
	3dc/win95/chnktexi.h \
	3dc/win95/chnktype.hpp \
	3dc/win95/chunk.hpp \
	3dc/win95/chunkpal.hpp \
	3dc/avp/support/command.hpp \
	3dc/avp/comp_shp.h \
	3dc/avp/support/consbind.hpp \
	3dc/avp/support/consbtch.hpp \
	3dc/avp/win95/gadgets/conscmnd.hpp \
	3dc/avp/consolelog.hpp \
	3dc/avp/win95/gadgets/conssym.hpp \
	3dc/avp/win95/gadgets/consvar.hpp \
	3dc/avp/support/coordstr.hpp \
	3dc/win95/d3_func.h \
	3dc/avp/win95/d3d_hud.h \
	3dc/avp/win95/d3d_render.h \
	3dc/win95/d3dmacs.h \
	3dc/avp/support/daemon.h \
	3dc/avp/database.h \
	3dc/avp/win95/datatype.h \
	3dc/avp/davehook.h \
	3dc/win95/db.h \
	3dc/avp/win95/dbdefs.h \
	3dc/avp/support/dcontext.hpp \
	3dc/win95/debuglog.h \
	3dc/win95/debuglog.hpp \
	3dc/avp/decal.h \
	3dc/avp/detaillevels.h \
	3dc/avp/win95/dp_func.h \
	3dc/avp/win95/dp_sprh.h \
	3dc/avp/win95/dplayext.h \
	3dc/win95/dummyobjectchunk.hpp \
	3dc/win95/dxlog.h \
	3dc/avp/dynamics.h \
	3dc/avp/dynblock.h \
	3dc/avp/win95/eax.h \
	3dc/avp/win95/endianio.h \
	3dc/win95/enumchnk.hpp \
	3dc/win95/enumsch.hpp \
	3dc/win95/envchunk.hpp \
	3dc/avp/equates.h \
	3dc/avp/equipmnt.h \
	3dc/avp/equiputl.hpp \
	3dc/avp/support/expvar.hpp \
	3dc/avp/extents.h \
	3dc/win95/fail.h \
	3dc/avp/win95/ffread.hpp \
	3dc/avp/win95/ffstdio.h \
	3dc/avp/win95/font.h \
	3dc/win95/fragchnk.hpp \
	3dc/frustrum.h \
	3dc/avp/win95/gadgets/gadget.h \
	3dc/avp/game_statistics.h \
	3dc/avp/gamedef.h \
	3dc/avp/gameflow.h \
	3dc/avp/win95/gameplat.h \
	3dc/avp/win95/gammacontrol.h \
	3dc/win95/gsprchnk.hpp \
	3dc/win95/hash_tem.hpp \
	3dc/avp/win95/heap_tem.hpp \
	3dc/win95/hierchnk.hpp \
	3dc/avp/win95/hierplace.hpp \
	3dc/avp/hmodel.h \
	3dc/avp/win95/hud_data.h \
	3dc/avp/win95/hud_layout.h \
	3dc/avp/hud_map.h \
	3dc/avp/huddefs.h \
	3dc/avp/win95/gadgets/hudgadg.hpp \
	3dc/avp/win95/hudgfx.h \
	3dc/win95/huffman.hpp \
	3dc/avp/win95/ia3d.h \
	3dc/win95/iff.hpp \
	3dc/win95/iff_ilbm.hpp \
	3dc/win95/ilbm_ext.hpp \
	3dc/avp/support/indexfnt.hpp \
	3dc/win95/inline.h \
	3dc/avp/win95/intro.hpp \
	3dc/avp/inventry.h \
	3dc/avp/win95/iofocus.h \
	3dc/avp/win95/jsndsup.h \
	3dc/kshape.h \
	3dc/avp/win95/kzsort.h \
	3dc/avp/langenum.h \
	3dc/avp/language.h \
	3dc/avp/lighting.h \
	3dc/win95/list_tem.hpp \
	3dc/avp/load_shp.h \
	3dc/avp/los.h \
	3dc/win95/ltchunk.hpp \
	3dc/avp/ltfx_exp.h \
	3dc/avp/macro.h \
	3dc/win95/md5.h \
	3dc/win95/media.hpp \
	3dc/avp/mempool.h \
	3dc/avp/menudefs.h \
	3dc/avp/win95/menugfx.h \
	3dc/win95/mishchnk.hpp \
	3dc/avp/missions.hpp \
	3dc/win95/mmx_math.h \
	3dc/avp/win95/modcmds.hpp \
	3dc/mslhand.h \
	3dc/avp/win95/multmenu.h \
	3dc/avp/win95/npcsetup.h \
	3dc/win95/obchunk.hpp \
	3dc/win95/objedit.h \
	3dc/avp/win95/objsetup.hpp \
	3dc/win95/oechunk.h \
	3dc/win95/ourasert.h \
	3dc/avp/support/ourbool.h \
	3dc/avp/paintball.h \
	3dc/avp/particle.h \
	3dc/avp/win95/pathchnk.hpp \
	3dc/avp/win95/pcmenus.h \
	3dc/win95/pentime.h \
	3dc/avp/pfarlocs.h \
	3dc/avp/pheromon.h \
	3dc/win95/plat_shp.h \
	3dc/win95/platform.h \
	3dc/avp/win95/pldghost.h \
	3dc/avp/win95/pldnet.h \
	3dc/avp/pmove.h \
	3dc/avp/win95/progress_bar.h \
	3dc/avp/projfont.h \
	3dc/avp/win95/projload.hpp \
	3dc/avp/projmenu.hpp \
	3dc/avp/projtext.h \
	3dc/avp/psnd.h \
	3dc/avp/win95/psndplat.h \
	3dc/avp/psndproj.h \
	3dc/avp/pvisible.h \
	3dc/avp/support/r2base.h \
	3dc/avp/support/r2pos666.hpp \
	3dc/win95/rad.h \
	3dc/avp/support/rebitems.hpp \
	3dc/avp/support/rebmenus.hpp \
	3dc/avp/support/reflist.hpp \
	3dc/avp/support/refobj.hpp \
	3dc/avp/support/rentrntq.h \
	3dc/avp/win95/gadgets/rootgadg.hpp \
	3dc/avp/savegame.h \
	3dc/avp/scream.h \
	3dc/avp/win95/scrshot.hpp \
	3dc/avp/support/scstring.hpp \
	3dc/avp/sequnces.h \
	3dc/avp/sfx.h \
	3dc/win95/showcmds.h \
	3dc/win95/shpanim.h \
	3dc/win95/shpchunk.hpp \
	3dc/win95/smack.h \
	3dc/win95/smacker.h \
	3dc/win95/smsopt.h \
	3dc/win95/sndchunk.hpp \
	3dc/sphere.h \
	3dc/win95/sprchunk.hpp \
	3dc/avp/statpane.h \
	3dc/avp/win95/strachnk.hpp \
	3dc/avp/stratdef.h \
	3dc/win95/string.hpp \
	3dc/avp/support/strtab.hpp \
	3dc/avp/support/strutil.h \
	3dc/avp/win95/system.h \
	3dc/avp/win95/gadgets/t_ingadg.hpp \
	3dc/avp/support/tallfont.hpp \
	3dc/avp/targeting.h \
	3dc/avp/win95/gadgets/teletype.hpp \
	3dc/avp/win95/gadgets/textexp.hpp \
	3dc/avp/win95/gadgets/textin.hpp \
	3dc/win95/toolchnk.hpp \
	3dc/avp/track.h \
	3dc/avp/win95/gadgets/trepgadg.hpp \
	3dc/avp/support/trig666.hpp \
	3dc/avp/triggers.h \
	3dc/win95/txioctrl.h \
	3dc/avp/win95/usr_io.h \
	3dc/version.h \
	3dc/win95/videomodes.h \
	3dc/avp/win95/vision.h \
	3dc/avp/win95/vmanpset.h \
	3dc/win95/vramtime.h \
	3dc/avp/weapons.h \
	3dc/win95/wpchunk.hpp \
	3dc/avp/support/wrapstr.hpp \
	3dc/win95/zmouse.h \
	3dc/win95/zsp.hpp
RESOURCE_FILES=

SRCS=$(SOURCE_FILES) $(HEADER_FILES) $(RESOURCE_FILES) 

OBJS=$(patsubst %.rc,%.res,$(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.c %.cc %.cpp %.cxx %.rc,$(SRCS)))))))

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET) AvP.dep

.PHONY: depends
depends:
	-$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $(filter %.c %.cc %.cpp %.cxx,$(SRCS)) > AvP.dep

-include AvP.dep

