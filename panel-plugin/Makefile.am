plugindir = $(libdir)/xfce4/panel/plugins
plugin_LTLIBRARIES = libappletnetwork.la

libappletnetwork_la_CPPFLAGS =									\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"			\
	@LIBXFCE4UI_CFLAGS@												\
	@LIBXFCE4PANEL_CFLAGS@  -g

libappletnetwork_la_LIBADD =		\
	@LIBXFCE4UI_LIBS@						\
	@LIBXFCE4PANEL_LIBS@

libappletnetwork_la_SOURCES =  \
	network.cc \
	network-plugin.c

libappletnetwork_la_LDFLAGS = 																					\
	-avoid-version 																											\
	-module 																														\
	-no-undefined 																											\
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

# .desktop file
#
desktop_in_files = applet-network.desktop.in

desktopdir = $(datadir)/xfce4/panel/plugins
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

EXTRA_DIST = $(desktop_in_files)

DISTCLEANFILES = $(desktop_DATA)

applet-network-desktop: applet-network-desktop.in
	cp $< $@
