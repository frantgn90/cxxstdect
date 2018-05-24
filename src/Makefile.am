include $(top_srcdir)/src/PATHS

bin_PROGRAMS = stdect
stdect_SOURCES = stdect.cpp
stdect_CPPFLAGS = -I$(tutils_dir) \
				  -I$(putils_dir) \
				  -I$(parse_dir) \
				  -I$(reduce_dir) \
				  -I$(loopsid_dir) \
				  -I$(loopsmg_dir) \
				  -I$(gui_dir) \
				  -I$(model_dir) \
				  -I$(mlpack_dir)/include \
				  $(BOOST_CPPFLAGS) \
				  $(WX_CPPFLAGS) \
				  -DBOOST_LOG_DYN_LINK

SUBDIRS = 1-parse 2-reduce 3-loops-id 4-loops-merge 5-gui model trace-utils

stdect_LDFLAGS = $(BOOST_LDFLAGS) \
				 -Wl,-rpath -Wl,$(mlpack_dir)/lib \
			     -L$(mlpack_dir)/lib -lmlpack \
			     $(BOOST_PROGRAM_OPTIONS_LIB) \
				 $(WX_LIBS) \
			     -lboost_log \
			     -lpthread \
				 -larmadillo
stdect_LDADD = $(parse_dir)/libparser.a \
			   $(reduce_dir)/libreducer.a \
			   $(loopsid_dir)/libloopsid.a \
			   $(loopsmg_dir)/libloopsmerge.a \
			   $(gui_dir)/libpseudocode.a \
			   $(tutils_dir)/libtraceutils.a \
			   $(model_dir)/libmpicall.a 
