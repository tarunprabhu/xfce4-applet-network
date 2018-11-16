AUTOMAKE_OPTIONS=subdir-objects

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
	Constants.cc \
	CSS.cc \
	CSSBuilder.cc \
	Defaults.cc \
	Device.cc \
	DeviceConfig.cc \
	DeviceOptions.cc \
	DeviceTooltip.cc \
	DeviceUI.cc \
	DeviceStats.cc \
	Dial.cc \
	Disk.cc \
	DiskOptions.cc \
	DiskStats.cc \
	DiskTooltip.cc \
	DiskUI.cc \
	GtkUtils.cc \
	Icons.cc \
	IDialog.cc \
	Network.cc \
	NetworkOptions.cc \
	NetworkStats.cc \
	NetworkTooltip.cc \
	NetworkUI.cc \
	PanelPlugin.cc \
	Plugin.cc \
	PluginOptions.cc \
	PluginConfig.cc \
	PluginUI.cc \
	Utils.cc \
	XfceUtils.cc \
	main.c

# Common sys files
libappletnetwork_la_SOURCES += \
	sys/common/PathPrivate.cc \
	sys/common/StatsReader.cc

if LINUX
libappletnetwork_la_SOURCES += \
	sys/linux/DiskStatsReader.cc \
	sys/linux/NetworkStatsReader.cc \
	sys/linux/System.cc \
	sys/linux/UniqueID.cc
endif

# TODO: Add support for *BSD
if BSD
libappletnetwork_la_SOURCES +=
endif

# TODO: Add support for OSX
if OSX
libappletnetwork_la_SOURCES +=
endif

# TODO: Add support for Windows (this is almost never going to happen)
if WINDOWS
libappletnetwork_la_SOURCES +=
endif 

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

applet-network.desktop: applet-network.desktop.in
	cp $< $@
