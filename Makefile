# Makefile - AvP.dsp

ifndef CFG
CFG=AvP - Win32 Debug
endif
CC=gcc
CFLAGS=
CXX=g++
CXXFLAGS=$(CFLAGS)
RC=windres -O COFF
ifeq "$(CFG)"  "AvP - Win32 Release"
CFLAGS+=-DWIN32 -D_WINDOWS -W -D_MBCS -fexceptions -O2 -Ic:/mssdk/include -I3dc -I3dc/avp -Dengine=1 -I3dc/avp/support -D__STDC__ -I3dc/avp/win95 -DAVP_DEBUG_VERSION -I3dc/avp/win95/frontend -I3dc/avp/win95/gadgets -I3dc/include -I3dc/win95
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
TARGET=avpprog.exe
LDFLAGS+=-Lc:/mssdk/lib -L3dc -Wl,--subsystem,windows
LIBS+=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lddraw -ldsound -ldplayx -ldinput -lsmackw32 -lbinkw32 -lwinmm
else
ifeq "$(CFG)"  "AvP - Win32 Debug"
CFLAGS+=-I3dc/include -I3dc/win95 -W -DWIN32 -fexceptions -g -O0 -Ic:/mssdk/include -D_DEBUG -I3dc -D_WINDOWS -I3dc/avp -D_MBCS -I3dc/avp/support -Dengine=1 -I3dc/avp/win95 -D__STDC__ -I3dc/avp/win95/frontend -DAVP_DEBUG_VERSION -I3dc/avp/win95/gadgets
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
TARGET=debug_AvP.exe
LDFLAGS+=-Lc:/mssdk/lib -L3dc -Wl,--subsystem,windows
LIBS+=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lddraw -ldsound -ldplayx -ldinput -lsmackw32 -lbinkw32 -lwinmm
else
ifeq "$(CFG)"  "AvP - Win32 Release For Fox"
CFLAGS+=-DWIN32 -D_WINDOWS -W -D_MBCS -fexceptions -O2 -Ic:/mssdk/include -I3dc -I3dc/avp -Dengine=1 -I3dc/avp/support -D__STDC__ -I3dc/avp/win95 -I3dc/avp/win95/frontend -I3dc/avp/win95/gadgets -I3dc/include -I3dc/win95
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
TARGET=AvP.exe
LDFLAGS+=-Lc:/mssdk/lib -L3dc -Wl,--subsystem,windows
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
	3dc/Afont.c \
	3dc/avp/win95/gadgets/ahudgadg.cpp \
	3dc/avp/AI_Sight.c \
	3dc/win95/alt_tab.cpp \
	3dc/win95/Animchnk.cpp \
	3dc/win95/animobs.cpp \
	3dc/avp/win95/Frontend/AvP_EnvInfo.c \
	3dc/avp/win95/Frontend/AvP_Intro.cpp \
	3dc/avp/win95/Frontend/AvP_MenuData.c \
	3dc/avp/win95/Frontend/AvP_MenuGfx.cpp \
	3dc/avp/win95/Frontend/AvP_Menus.c \
	3dc/avp/win95/Frontend/AvP_MP_Config.cpp \
	3dc/avp/win95/Frontend/AvP_UserProfile.cpp \
	3dc/avp/win95/Avpchunk.cpp \
	3dc/avp/win95/AvpReg.cpp \
	3dc/avp/Avpview.c \
	3dc/win95/awBmpLd.cpp \
	3dc/win95/awIffLd.cpp \
	3dc/win95/awPnmLd.cpp \
	3dc/win95/awTexLd.cpp \
	3dc/avp/bh_agun.c \
	3dc/avp/bh_ais.c \
	3dc/avp/Bh_alien.c \
	3dc/avp/Bh_binsw.c \
	3dc/avp/bh_cable.c \
	3dc/avp/bh_corpse.c \
	3dc/avp/bh_deathvol.c \
	3dc/avp/Bh_debri.c \
	3dc/avp/bh_dummy.c \
	3dc/avp/bh_fan.c \
	3dc/avp/bh_far.c \
	3dc/avp/Bh_fhug.c \
	3dc/avp/Bh_gener.c \
	3dc/avp/bh_ldoor.c \
	3dc/avp/bh_lift.c \
	3dc/avp/bh_light.c \
	3dc/avp/Bh_lnksw.c \
	3dc/avp/bh_ltfx.c \
	3dc/avp/Bh_marin.c \
	3dc/avp/bh_mission.c \
	3dc/avp/Bh_near.c \
	3dc/avp/bh_pargen.c \
	3dc/avp/bh_plachier.c \
	3dc/avp/bh_plift.c \
	3dc/avp/Bh_pred.c \
	3dc/avp/bh_queen.c \
	3dc/avp/bh_RubberDuck.c \
	3dc/avp/bh_selfdest.c \
	3dc/avp/bh_snds.c \
	3dc/avp/bh_spcl.c \
	3dc/avp/Bh_swdor.c \
	3dc/avp/bh_track.c \
	3dc/avp/Bh_types.c \
	3dc/avp/bh_videoscreen.c \
	3dc/avp/bh_waypt.c \
	3dc/avp/bh_weap.c \
	3dc/avp/Bh_xeno.c \
	3dc/win95/bink.c \
	3dc/win95/Bmpnames.cpp \
	3dc/avp/BonusAbilities.c \
	3dc/avp/cconvars.cpp \
	3dc/win95/CD_player.c \
	3dc/avp/CDTrackSelection.cpp \
	3dc/avp/win95/Cheat.c \
	3dc/avp/CheatModes.c \
	3dc/win95/chnkload.cpp \
	3dc/win95/Chnktexi.cpp \
	3dc/win95/Chnktype.cpp \
	3dc/avp/win95/chtcodes.cpp \
	3dc/win95/Chunk.cpp \
	3dc/win95/Chunkpal.cpp \
	3dc/avp/comp_map.c \
	3dc/avp/Comp_shp.c \
	3dc/avp/support/consbind.cpp \
	3dc/avp/support/consbtch.cpp \
	3dc/avp/win95/gadgets/conscmnd.cpp \
	3dc/avp/ConsoleLog.cpp \
	3dc/avp/win95/gadgets/conssym.cpp \
	3dc/avp/win95/gadgets/consvar.cpp \
	3dc/avp/support/Coordstr.cpp \
	3dc/avp/shapes/Cube.c \
	3dc/win95/d3_func.cpp \
	3dc/avp/win95/d3d_hud.cpp \
	3dc/avp/win95/d3d_render.cpp \
	3dc/avp/support/Daemon.cpp \
	3dc/avp/davehook.cpp \
	3dc/win95/db.c \
	3dc/win95/Dd_func.cpp \
	3dc/avp/win95/Ddplat.cpp \
	3dc/avp/deaths.c \
	3dc/win95/Debuglog.cpp \
	3dc/avp/decal.c \
	3dc/avp/DetailLevels.c \
	3dc/win95/Di_func.cpp \
	3dc/avp/win95/DirectPlay.c \
	3dc/avp/win95/Dp_func.c \
	3dc/avp/win95/dplayext.c \
	3dc/win95/DummyObjectChunk.cpp \
	3dc/avp/win95/dx_proj.cpp \
	3dc/win95/Dxlog.c \
	3dc/avp/Dynamics.c \
	3dc/avp/Dynblock.c \
	3dc/avp/win95/endianio.c \
	3dc/win95/Enumchnk.cpp \
	3dc/win95/Enumsch.cpp \
	3dc/win95/Envchunk.cpp \
	3dc/avp/Equipmnt.c \
	3dc/avp/equiputl.cpp \
	3dc/avp/extents.c \
	3dc/win95/fail.c \
	3dc/avp/win95/Ffread.cpp \
	3dc/avp/win95/Ffstdio.cpp \
	3dc/win95/fragchnk.cpp \
	3dc/frustrum.c \
	3dc/avp/win95/gadgets/gadget.cpp \
	3dc/avp/Game.c \
	3dc/avp/game_statistics.c \
	3dc/avp/gamecmds.cpp \
	3dc/avp/gameflow.c \
	3dc/avp/gamevars.cpp \
	3dc/avp/win95/GammaControl.cpp \
	3dc/avp/win95/gflwplat.c \
	3dc/win95/Gsprchnk.cpp \
	3dc/win95/hierchnk.cpp \
	3dc/avp/win95/hierplace.cpp \
	3dc/avp/HModel.c \
	3dc/avp/Hud.c \
	3dc/avp/win95/gadgets/hudgadg.cpp \
	3dc/win95/huffman.cpp \
	3dc/win95/iff.cpp \
	3dc/win95/iff_ILBM.cpp \
	3dc/win95/ILBM_ext.cpp \
	3dc/avp/support/indexfnt.cpp \
	3dc/avp/win95/intro.cpp \
	3dc/avp/Inventry.c \
	3dc/win95/io.c \
	3dc/avp/win95/iofocus.cpp \
	3dc/avp/win95/jsndsup.cpp \
	3dc/Kshape.c \
	3dc/avp/win95/Kzsort.c \
	3dc/avp/win95/Langplat.c \
	3dc/avp/Language.c \
	3dc/avp/Lighting.c \
	3dc/win95/list_tem.cpp \
	3dc/avp/load_shp.c \
	3dc/avp/los.c \
	3dc/win95/Ltchunk.cpp \
	3dc/Map.c \
	3dc/avp/Maps.c \
	3dc/Maths.c \
	3dc/win95/md5.c \
	3dc/win95/media.cpp \
	3dc/mem3dc.c \
	3dc/Mem3dcpp.cpp \
	3dc/avp/mempool.c \
	3dc/avp/MessageHistory.c \
	3dc/win95/Mishchnk.cpp \
	3dc/avp/missions.cpp \
	3dc/avp/win95/modcmds.cpp \
	3dc/Module.c \
	3dc/Morph.c \
	3dc/avp/win95/MouseCentreing.cpp \
	3dc/avp/movement.c \
	3dc/avp/mp_launch.c \
	3dc/Mslhand.c \
	3dc/avp/win95/Npcsetup.cpp \
	3dc/win95/Obchunk.cpp \
	3dc/Object.c \
	3dc/avp/win95/Objsetup.cpp \
	3dc/win95/OEChunk.cpp \
	3dc/win95/Our_mem.c \
	3dc/avp/Paintball.c \
	3dc/avp/particle.c \
	3dc/avp/win95/PathChnk.cpp \
	3dc/avp/win95/Pcmenus.cpp \
	3dc/avp/Pfarlocs.c \
	3dc/avp/Pheromon.c \
	3dc/win95/plat_shp.c \
	3dc/avp/win95/Platsup.c \
	3dc/avp/Player.c \
	3dc/avp/win95/Pldghost.c \
	3dc/avp/win95/Pldnet.c \
	3dc/win95/plspecfn.c \
	3dc/avp/Pmove.c \
	3dc/avp/win95/progress_bar.cpp \
	3dc/avp/win95/Projload.cpp \
	3dc/avp/Psnd.c \
	3dc/avp/win95/Psndplat.c \
	3dc/avp/Psndproj.c \
	3dc/avp/Pvisible.c \
	3dc/avp/support/r2base.cpp \
	3dc/avp/support/r2pos666.cpp \
	3dc/avp/support/reflist.cpp \
	3dc/avp/support/refobj.cpp \
	3dc/avp/support/rentrntq.cpp \
	3dc/avp/win95/gadgets/rootgadg.cpp \
	3dc/avp/savegame.c \
	3dc/avp/scream.cpp \
	3dc/avp/win95/Scrshot.cpp \
	3dc/avp/support/scstring.cpp \
	3dc/avp/SecStats.c \
	3dc/avp/sfx.c \
	3dc/shpanim.c \
	3dc/win95/Shpchunk.cpp \
	3dc/win95/smacker.c \
	3dc/win95/Sndchunk.cpp \
	3dc/sphere.c \
	3dc/win95/Sprchunk.cpp \
	3dc/avp/win95/Strachnk.cpp \
	3dc/avp/Stratdef.c \
	3dc/win95/String.cpp \
	3dc/avp/support/strtab.cpp \
	3dc/avp/support/strutil.c \
	3dc/avp/win95/system.c \
	3dc/avp/win95/gadgets/t_ingadg.cpp \
	3dc/Tables.c \
	3dc/avp/support/tallfont.cpp \
	3dc/avp/targeting.c \
	3dc/avp/win95/gadgets/teletype.cpp \
	3dc/win95/Texio.c \
	3dc/avp/win95/gadgets/textexp.cpp \
	3dc/avp/win95/gadgets/textin.cpp \
	3dc/win95/Toolchnk.cpp \
	3dc/avp/track.c \
	3dc/avp/win95/gadgets/trepgadg.cpp \
	3dc/avp/support/trig666.cpp \
	3dc/avp/Triggers.c \
	3dc/win95/Txioctrl.cpp \
	3dc/avp/win95/Usr_io.c \
	3dc/Vdb.c \
	3dc/version.c \
	3dc/win95/VideoModes.cpp \
	3dc/avp/win95/Vision.c \
	3dc/win95/Vramtime.c \
	3dc/avp/Weapons.c \
	3dc/win95/win_func.cpp \
	3dc/avp/win95/win_proj.cpp \
	3dc/avp/win95/winmain.c \
	3dc/win95/wpchunk.cpp \
	3dc/avp/support/wrapstr.cpp \
	3dc/win95/Zsp.cpp

HEADER_FILES= \
	3dc/win95/advwin32.h \
	3dc/avp/win95/gadgets/ahudgadg.hpp \
	3dc/avp/AI_Sight.h \
	3dc/win95/alt_tab.h \
	3dc/win95/Animchnk.hpp \
	3dc/win95/animobs.hpp \
	3dc/avp/win95/Frontend/AvP_EnvInfo.h \
	3dc/avp/win95/Frontend/AvP_MenuGfx.hpp \
	3dc/avp/win95/Frontend/AvP_Menus.h \
	3dc/avp/win95/Frontend/AvP_MP_Config.h \
	3dc/avp/win95/Frontend/AvP_UserProfile.h \
	3dc/avp/win95/Avpchunk.hpp \
	3dc/avp/avpitems.hpp \
	3dc/avp/avppages.hpp \
	3dc/avp/win95/AvpReg.hpp \
	3dc/avp/Avpview.h \
	3dc/win95/aw.h \
	3dc/win95/awTexLd.h \
	3dc/win95/awTexLd.hpp \
	3dc/avp/bh_agun.h \
	3dc/avp/bh_ais.h \
	3dc/avp/Bh_alien.h \
	3dc/avp/Bh_binsw.h \
	3dc/avp/bh_cable.h \
	3dc/avp/bh_corpse.h \
	3dc/avp/bh_deathvol.h \
	3dc/avp/bh_debri.h \
	3dc/avp/bh_dummy.h \
	3dc/avp/bh_fan.h \
	3dc/avp/Bh_far.h \
	3dc/avp/Bh_fhug.h \
	3dc/avp/Bh_gener.h \
	3dc/avp/bh_ldoor.h \
	3dc/avp/bh_lift.h \
	3dc/avp/bh_light.h \
	3dc/avp/Bh_lnksw.h \
	3dc/avp/bh_ltfx.h \
	3dc/avp/Bh_marin.h \
	3dc/avp/bh_mission.h \
	3dc/avp/Bh_near.h \
	3dc/avp/Bh_paq.h \
	3dc/avp/bh_pargen.h \
	3dc/avp/bh_plachier.h \
	3dc/avp/bh_plift.h \
	3dc/avp/Bh_pred.h \
	3dc/avp/bh_queen.h \
	3dc/avp/bh_RubberDuck.h \
	3dc/avp/bh_selfdest.h \
	3dc/avp/bh_snds.h \
	3dc/avp/bh_spcl.h \
	3dc/avp/Bh_swdor.h \
	3dc/avp/bh_track.h \
	3dc/avp/Bh_types.h \
	3dc/avp/bh_videoscreen.h \
	3dc/avp/bh_waypt.h \
	3dc/avp/bh_weap.h \
	3dc/avp/Bh_xeno.h \
	3dc/win95/bink.h \
	3dc/win95/bink_Rad.h \
	3dc/avp/win95/Bmp2.h \
	3dc/win95/Bmpnames.hpp \
	3dc/avp/BonusAbilities.h \
	3dc/win95/CD_player.h \
	3dc/avp/CDTrackSelection.h \
	3dc/avp/win95/Cheat.h \
	3dc/win95/Chnkload.h \
	3dc/win95/Chnkload.hpp \
	3dc/win95/Chnktexi.h \
	3dc/win95/Chnktype.hpp \
	3dc/win95/Chunk.hpp \
	3dc/win95/Chunkpal.hpp \
	3dc/avp/support/command.hpp \
	3dc/avp/Comp_shp.h \
	3dc/avp/support/consbind.hpp \
	3dc/avp/support/consbtch.hpp \
	3dc/avp/win95/gadgets/conscmnd.hpp \
	3dc/avp/ConsoleLog.hpp \
	3dc/avp/win95/gadgets/conssym.hpp \
	3dc/avp/win95/gadgets/consvar.hpp \
	3dc/avp/support/Coordstr.hpp \
	3dc/win95/d3_func.h \
	3dc/avp/win95/d3d_hud.h \
	3dc/avp/win95/d3d_render.h \
	3dc/win95/d3dmacs.h \
	3dc/avp/support/daemon.h \
	3dc/avp/Database.h \
	3dc/avp/win95/datatype.h \
	3dc/avp/davehook.h \
	3dc/win95/Db.h \
	3dc/avp/win95/dbdefs.h \
	3dc/avp/support/dcontext.hpp \
	3dc/win95/Debuglog.h \
	3dc/win95/Debuglog.hpp \
	3dc/avp/decal.h \
	3dc/avp/DetailLevels.h \
	3dc/avp/win95/Dp_func.h \
	3dc/avp/win95/dp_Sprh.h \
	3dc/avp/win95/dplayext.h \
	3dc/win95/DummyObjectChunk.hpp \
	3dc/win95/Dxlog.h \
	3dc/avp/Dynamics.h \
	3dc/avp/Dynblock.h \
	3dc/avp/win95/Eax.h \
	3dc/avp/win95/endianio.h \
	3dc/win95/Enumchnk.hpp \
	3dc/win95/Enumsch.hpp \
	3dc/win95/Envchunk.hpp \
	3dc/avp/Equates.h \
	3dc/avp/Equipmnt.h \
	3dc/avp/equiputl.hpp \
	3dc/avp/support/expvar.hpp \
	3dc/avp/extents.h \
	3dc/win95/fail.h \
	3dc/avp/win95/Ffread.hpp \
	3dc/avp/win95/Ffstdio.h \
	3dc/avp/win95/font.h \
	3dc/win95/fragchnk.hpp \
	3dc/frustrum.h \
	3dc/avp/win95/gadgets/gadget.h \
	3dc/avp/game_statistics.h \
	3dc/avp/Gamedef.h \
	3dc/avp/gameflow.h \
	3dc/avp/win95/Gameplat.h \
	3dc/avp/win95/GammaControl.h \
	3dc/win95/Gsprchnk.hpp \
	3dc/win95/Hash_tem.hpp \
	3dc/avp/win95/Heap_tem.hpp \
	3dc/win95/hierchnk.hpp \
	3dc/avp/win95/hierplace.hpp \
	3dc/avp/hmodel.h \
	3dc/avp/win95/Hud_data.h \
	3dc/avp/win95/HUD_layout.h \
	3dc/avp/Hud_map.h \
	3dc/avp/Huddefs.h \
	3dc/avp/win95/gadgets/hudgadg.hpp \
	3dc/avp/win95/Hudgfx.h \
	3dc/win95/huffman.hpp \
	3dc/avp/win95/Ia3d.h \
	3dc/win95/iff.hpp \
	3dc/win95/iff_ILBM.hpp \
	3dc/win95/ILBM_ext.hpp \
	3dc/avp/support/indexfnt.hpp \
	3dc/win95/Inline.h \
	3dc/avp/win95/intro.hpp \
	3dc/avp/Inventry.h \
	3dc/avp/win95/iofocus.h \
	3dc/avp/win95/jsndsup.h \
	3dc/Kshape.h \
	3dc/avp/win95/Kzsort.h \
	3dc/avp/langenum.h \
	3dc/avp/Language.h \
	3dc/avp/Lighting.h \
	3dc/win95/list_tem.hpp \
	3dc/avp/load_shp.h \
	3dc/avp/los.h \
	3dc/win95/Ltchunk.hpp \
	3dc/avp/ltfx_exp.h \
	3dc/avp/Macro.h \
	3dc/win95/md5.h \
	3dc/win95/media.hpp \
	3dc/avp/mempool.h \
	3dc/avp/menudefs.h \
	3dc/avp/win95/menugfx.h \
	3dc/win95/Mishchnk.hpp \
	3dc/avp/missions.hpp \
	3dc/win95/Mmx_math.h \
	3dc/avp/win95/modcmds.hpp \
	3dc/Mslhand.h \
	3dc/avp/win95/Multmenu.h \
	3dc/avp/win95/Npcsetup.h \
	3dc/win95/Obchunk.hpp \
	3dc/win95/objedit.h \
	3dc/avp/win95/Objsetup.hpp \
	3dc/win95/OEChunk.h \
	3dc/win95/Ourasert.h \
	3dc/avp/support/ourbool.h \
	3dc/avp/Paintball.h \
	3dc/avp/particle.h \
	3dc/avp/win95/PathChnk.hpp \
	3dc/avp/win95/Pcmenus.h \
	3dc/win95/Pentime.h \
	3dc/avp/Pfarlocs.h \
	3dc/avp/Pheromon.h \
	3dc/win95/plat_shp.h \
	3dc/win95/platform.h \
	3dc/avp/win95/Pldghost.h \
	3dc/avp/win95/Pldnet.h \
	3dc/avp/Pmove.h \
	3dc/avp/win95/progress_bar.h \
	3dc/avp/projfont.h \
	3dc/avp/win95/Projload.hpp \
	3dc/avp/projmenu.hpp \
	3dc/avp/projtext.h \
	3dc/avp/Psnd.h \
	3dc/avp/win95/Psndplat.h \
	3dc/avp/Psndproj.h \
	3dc/avp/Pvisible.h \
	3dc/avp/support/r2base.h \
	3dc/avp/support/r2pos666.hpp \
	3dc/win95/Rad.h \
	3dc/avp/support/rebitems.hpp \
	3dc/avp/support/rebmenus.hpp \
	3dc/avp/support/reflist.hpp \
	3dc/avp/support/refobj.hpp \
	3dc/avp/support/rentrntq.h \
	3dc/avp/win95/gadgets/rootgadg.hpp \
	3dc/avp/savegame.h \
	3dc/avp/scream.h \
	3dc/avp/win95/Scrshot.hpp \
	3dc/avp/support/scstring.hpp \
	3dc/avp/sequnces.h \
	3dc/avp/sfx.h \
	3dc/win95/ShowCmds.h \
	3dc/win95/shpanim.h \
	3dc/win95/Shpchunk.hpp \
	3dc/win95/Smack.h \
	3dc/win95/smacker.h \
	3dc/win95/Smsopt.h \
	3dc/win95/Sndchunk.hpp \
	3dc/sphere.h \
	3dc/win95/Sprchunk.hpp \
	3dc/avp/statpane.h \
	3dc/avp/win95/Strachnk.hpp \
	3dc/avp/Stratdef.h \
	3dc/win95/String.hpp \
	3dc/avp/support/strtab.hpp \
	3dc/avp/support/strutil.h \
	3dc/avp/win95/System.h \
	3dc/avp/win95/gadgets/t_ingadg.hpp \
	3dc/avp/support/tallfont.hpp \
	3dc/avp/targeting.h \
	3dc/avp/win95/gadgets/teletype.hpp \
	3dc/avp/win95/gadgets/textexp.hpp \
	3dc/avp/win95/gadgets/textin.hpp \
	3dc/win95/Toolchnk.hpp \
	3dc/avp/track.h \
	3dc/avp/win95/gadgets/trepgadg.hpp \
	3dc/avp/support/trig666.hpp \
	3dc/avp/Triggers.h \
	3dc/win95/Txioctrl.h \
	3dc/avp/win95/Usr_io.h \
	3dc/version.h \
	3dc/win95/VideoModes.h \
	3dc/avp/win95/Vision.h \
	3dc/avp/win95/Vmanpset.h \
	3dc/win95/Vramtime.h \
	3dc/avp/Weapons.h \
	3dc/win95/wpchunk.hpp \
	3dc/avp/support/wrapstr.hpp \
	3dc/win95/Zmouse.h \
	3dc/win95/Zsp.hpp

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

