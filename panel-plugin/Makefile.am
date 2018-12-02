AUTOMAKE_OPTIONS=subdir-objects

plugindir = $(libdir)/xfce4/panel/plugins
plugin_LTLIBRARIES = libappletnetwork.la

libappletnetwork_la_CPPFLAGS =									\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"			\
	@LIBXFCE4UI_CFLAGS@												\
	@LIBXFCE4PANEL_CFLAGS@ \
	@GTKMM_CFLAGS@

libappletnetwork_la_LIBADD =		\
	@LIBXFCE4UI_LIBS@						\
	@LIBXFCE4PANEL_LIBS@ \
	@GTKMM_LIBS@

libappletnetwork_la_SOURCES =  \
	BinaryPrefix.cc \
	Config.cc \
	Constants.cc \
	CSSBuilder.cc \
	Defaults.cc \
	Device.cc \
	DeviceConfigDialog.cc \
	DeviceTooltip.cc \
	DeviceWidget.cc \
	DeviceStats.cc \
	Disk.cc \
	DiskStats.cc \
	DiskTooltip.cc \
	Formatter.cc \
	FrameWidget.cc \
	GridWidget.cc \
	GtkmmUtils.cc \
	Icons.cc \
	LabelWidget.cc \
	MetricPrefix.cc \
	Network.cc \
	NetworkStats.cc \
	NetworkTooltip.cc \
	PanelPlugin.cc \
	Plugin.cc \
	PluginConfigDialog.cc \
	PluginWidget.cc \
	ToggleButtonWidget.cc \
	ScaleWidget.cc \
	UnitPrefix.cc \
	Utils.cc \
	XfcePanelPlugin.cc \
	XfceRc.cc \
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
