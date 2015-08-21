Summary: Gstreamer application example using playbin2
Name: %{_stm_pkg_prefix}-target-gst-apps
Obsoletes: %{_stm_pkg_prefix}-gst-apps <= 1.0.6-1
Version: 1.0.75
Release: 1
License: Proprietary
Group: Applications/Multimedia
Source0: gst-apps.tar.bz2
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-%{_stm_target_arch}-XXXXXX)
BuildRequires: %{_stm_pkg_prefix}-%{_stm_target_arch}-gstreamer >= 0.10.34
BuildRequires: %{_stm_pkg_prefix}-%{_stm_target_arch}-gst-plugins-base >= 0.10.34
Prefix: %{_stm_cross_target_dir}

%define _description \
This module contains a gstreamer application using playbin2. This provides a Media Player, a Zapper and a Live Recorder. You need the GStreamer core module \
and gst-plugins-base to make them work...

Summary: Gstreamer application
Group: Applications/Multimedia
%description 
%{_description}

%ifarch noarch

%define _pkgname %{_stm_pkg_prefix}-host-gst-apps-source
%package -n %{_pkgname}
Summary: Gstreamer application source code
Group: Applications/Multimedia
Requires: %{_stm_pkg_prefix}-host-filesystem
AutoReqProv: no

%description -n %{_pkgname}
%{_description}
This package contains the source code

%else

%define _defaultdocdir       %{_stm_cross_target_dir}%{_stm_target_doc_dir}
%define _pkgname             %{_stm_pkg_prefix}-%{_stm_target_arch}-gst-apps

%package -n %{_pkgname}
Summary: Gstreamer application
Group: Applications/Multimedia

%description -n %{_pkgname}
%{_description}

%endif

%prep
%setup -q -n gst-apps

%build
%target_setup

%ifarch noarch
%else
# We have to do very wierd things here. When we run autoconf,
# it overwrites the po/Makefile.in.in and deletes the GETTEXT_PACKAGE
# macro. Clearly this is a common problem, because there is a patch
# in the source tree to fix it up. Should it be in gettext instead?
%target_autoreconf

%target_configure

%pmake
%endif

%install
%target_setup

%ifarch noarch
install -d -m 755 %{buildroot}%{_stm_sources_dir}/gst-apps/gst-apps-%{version}-%{release}
tar chf - . | tar xf - -C %{buildroot}%{_stm_sources_dir}/gst-apps/gst-apps-%{version}-%{release}
%else
mkdir -p %{buildroot}%{_stm_cross_target_dir}%{_stm_target_data_dir}/applications/gst-apps
install -m 644 src/gst-apps.lircrc %{buildroot}%{_stm_cross_target_dir}%{_stm_target_data_dir}/applications/gst-apps/.
%target_makeinstall

cd ..
%endif

%clean
rm -rf %{buildroot}

%ifarch noarch
%post -n %{_pkgname}
# Since it is not an error to parallel install gst-apps, we don't want the
# link to the 'latest' to be part of the spec file.
rm -f %{_stm_sources_dir}/gst-apps/gst-apps
ln -s gst-apps-%{version}-%{release} %{_stm_sources_dir}/gst-apps/gst-apps

%preun -n %{_pkgname}
if [ x`readlink %{_stm_sources_dir}/gst-apps/gst-apps` = xgst-apps-%{version}-%{release} ] ; then
	rm -f %{_stm_sources_dir}/gst-apps/gst-apps
fi
%endif

%ifarch noarch

%files -n %{_pkgname}
%defattr(-,root,root)
%{_stm_sources_dir}/gst-apps/gst-apps-%{version}-%{release}

%else
%files -n %{_pkgname}
%defattr(-, root, root)
%{_stm_cross_target_dir}%{_stm_target_bin_dir}/gst-apps
%{_stm_cross_target_dir}%{_stm_target_data_dir}/applications/gst-apps/gst-apps.lircrc

%doc README
%endif

%changelog
* Thu Nov 21 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.75
- [bugzilla : 40503] for audio only streams trickmode is supported only for 0 to 2X
* Thu Nov 14 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.74
- cpu consumption is too high
* Thu Oct 31 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.73
- [bugzilla : 38745] [DVR] record option should not try to record in a recorded file playback
- [bugzilla : 36905] audio loss observed randomly in Direct Catch LIVE with '@' cmd
- [bugzilla : 39118] Add "i state" option in gst-apps to debug states
- [bugzilla : 39015] DVR-record-and-timeshift-commands-should not interfere
- [bugzilla : 17175] avaoid same speed change except catch live
* Thu Oct 24 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.72
- [bugzilla : 38859] set_playback_latency_for_staudio
- [bugzilla : 38634] http_ts_reverse
- [bugzilla : 38711] Multiple injection by ipfe
* Fri Oct 11 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.71
- [bugzilla : 28050] app should fallback to sw jpegdec if hw jpegdec fails
- [bugzilla : 35082] [DVR/Non DVR]speed change makes paused playback resume
* Fri Oct 04 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.70
- [bugzilla : 16369] gst-apps does not support jpeg navigation
* Tue Oct 01 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.69
- [bugzilla : 28050] Positioning set_property for user defined plane
* Thu Sep 19 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.68
- [bugzilla : 36167] [DVR] display time info in timeshift
- [bugzilla : 32479] chanel-change property not supported for tsdemux
- [bugzilla : 28850] seek handling in application
* Wed Aug 28 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.67
- [bugzilla : 28050] Positioning set_property for user defined plane
* Tue Aug 27 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.66
- [bugzilla : 31487] stdvbbin: add a property to specify channel name
* Wed Aug 21 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.65
- [bugzilla : 34615] seek after pause on a recorded playback results in resume
- [bugzilla : 31343] Switching stdisplaysink from PLAYING->READY->PLAYING has issue
* Wed Aug 07 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.64
- [bugzilla : 34655] gst_object_unref not called in gstUserInterface.c if t or u/d is performed
* Thu Aug 01 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.63
- [bugzilla : 31360] dispose and finalize not call when exiting gst-apps
- [bugzilla : 30117] diseqc_switch_mechanisn_and_no-lnb_and_no-diseqc_option
* Fri Jul 19 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.62
- [bugzilla : 32976]  GST-APPS recorded playback interface issue
* Thu Jul 11 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.61
- [bugzilla : 30364]  Not able to run a live playback on a grab decode
* Fri Jun 28 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.60
- [bugzilla : 32619]  SDK2Build.mak: Repair Arch verification line
* Tue Jun 25 2013 Saurabh Kumar AWASTHI <saurabh.awasthi@st.com> - v1.0.59
- [bugzilla : 30454]  Remove factories list manipulation for MJPEG/JPEG codec(modified)
* Thu May 30 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.58
- [bugzilla : 30454]  Remove factories list manipulation for MJPEG/JPEG codec
- [bugzilla : 22835]  start of stream hault handling
* Mon May 20 2013 Saurabh Kumar AWASTHI <saurabh.awasthi@st.com> - v1.0.57
- [bugzilla : 20344]  configure: check LIRC header and library
- [bugzilla : 29603]  trace option added in gst-apps
* Wed May 08 2013 sawasthi <saurabh.awasthi@st.com> - v1.0.56
- [bugzilla : 29404]  [STMFApp]Fix coverity issues
- [bugzilla : 28563]  stdisplaysink should override the ::show_frame vmethod instead of ::render
* Wed Apr 24 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.55
- [bugzilla : 29564]  STMFApp: use correct automake variables for linking libraries
* Wed Apr 17 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.54
- [bugzilla : 29244]  STMFApp: update config.h after ./configure
- [bugzilla : 29242]  STMFApp: build_module.sh must stop in case of error
- [bugzilla : 28994]  APPS_STMFApp scrambled stream handling
* Wed Apr 10 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.53
- [bugzilla : 29101]  fix bash'ism in build_module.sh
- [bugzilla : 28512]  state change message removed
* Mon Apr 08 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.52
- [bugzilla : 28793]  STMFApp: add lirc file to the list of file
- [bugzilla : 28683]  http playback : blocksize property setting
- [bugzilla : 28779]  STMFApp: sdk2build.mak variables conflict
* Thu Apr 04 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.51
- [bugzilla : 28795]  Makefile: autogen.sh is no more distributed
- [bugzilla : 28792]  Makefile: add missing header to sources
* Fri Mar 22 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.50
- [bugzilla : 26458]  added suppot for set speed 128 to -128
* Fri Mar 15 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.49
- [bugzilla : 27761] RAP Indexing now workign for IPFE
* Fri Mar 08 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.48
- [bugzilla : 24706] removed the stts_demux prop set for dev-id
- [bugzilla : 20720] Avoid_Extra_Caps_Unref
- [bugzilla : 26146] direct time-shift to live not working (no resume)
* Mon Mar 04 2013 sawasthi <sawasthi@DLH20744.dlh.st.com> - v1.0.47
- [bugzilla : 26010] dual mono support in gst-apps
- [bugzilla : 26853] ltmain.sh and libtool are no more produced
* Tue Feb 26 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.46
- [bugzilla : 26260] build_module.sh: remove the here-doc script
- [bugzilla : 27319] README.{arm,sh4}: don't ask autoreconf to do the build
- [bugzilla : 27143] Use DESTDIR instead of --prefix to install the package in devkit/target
- [bugzilla : 27139] removed unneeded autogen.sh
* Tue Feb 19 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.45
- [bugzilla : 26876] replace stop with blue button for stm_pvr_1 remote
* Fri Feb 15 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.42
- [bugzilla : 26164] Add flexibility in target FS location for app compilation
- [bugzilla : 26390] Grabfile test failing for "120Hz_650_DEMO.ts"
- [bugzilla : 26853] configure.ac: remove libtool
- [bugzilla : 26854] Fix git ignore list
* Thu Jan 24 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.41
- [bugzilla : 26269] transp property not working
* Fri Jan 11 2013 Pavan Goyal <pavan.goyal@st.com> - v1.0.40
- [bugzilla : 25170] quit app if tuner lock fails
- [bugzilla : 25318] stvideo: Expose player2 AVSync control option
- [bugzilla : 25263] Fix build issues in mock environment when not having stlinux installed in host
- [bugzilla : 22181] reverse trickmode enabled in timeshift
* Wed Dec 19 2012 Pavan Goyal <pavan.goyal@st.com> - v1.0.39
- [bugzilla : 25003] current_playing_info_indented
* Fri Nov 30 2012 Pavan Goyal <pavan.goyal@st.com> - v1.0.38
- [bugzilla : 23130] Gst-apps keyboard polling excessive CPU consumption
* Thu Nov 08 2012 Pavan Goyal <pavan.goyal@st.com> - v1.0.36
- [bugzilla : 15234] Track change support Added
* Mon Nov 05 2012 Pavan Goyal <pavan.goyal@st.com> - v1.0.35
- [bugzilla : 22180] [PVR]SEEK support added in TimeShift 
- [bugzilla : 21407] [PVR]circular buffer handling provided in DVR  
- [bugzilla : 23639] eos message is not coming for all streams playback after moving to gstreamer-0.10.36
* Fri Oct 26 2012 Pavan Goyal <pavan.goyal@st.com> - v1.0.34
- [bugzilla : 23652] Add stmtools dir to build gst-apps 
- [bugzilla : 23468] Multifile_source_restructuring_into_library
- [bugzilla : 22742] trick mode and seek are affecting Live channel decode
* Tue Oct 16 2012 Pavan Goyal <pavan.goyal@st.com> - v1.0.33
- [bugzilla : 20715] gstreamer element clean-up
- [bugzilla : 22962] Interlacing issue observed if flip operation is done after rotation in either direction
- [bugzilla : 23172] gst tasks to be named
- [bugzilla : 22473] gstreamer plugin should rely on the H/W demux for both 188 & 192 TS packets
