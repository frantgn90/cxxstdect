include $(top_srcdir)/src/PATHS

noinst_LIBRARIES = libloopsid.a
libloopsid_a_SOURCES = loops_identification.cpp 

libloopsid_a_CPPFLAGS = -I$(model_dir) \
						-I$(reduce_dir) \
						-I$(parse_dir) \
						-I$(putils_dir) \
						-I$(tutils_dir) \
						-I$(mlpack_dir)/include \
						$(BOOST_CPPFLAGS) \
						$(BOOST_LDFLAGS)
