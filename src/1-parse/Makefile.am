include $(top_srcdir)/src/PATHS

noinst_LIBRARIES = libparser.a
libparser_a_SOURCES = nptrace.cpp nprecord.cpp

libparser_a_CPPFLAGS = -I$(tutils_dir) \
					   -I$(putils_dir) \
						$(BOOST_CPPFLAGS) \
						-lz
