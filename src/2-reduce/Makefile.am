include $(top_srcdir)/src/PATHS

noinst_LIBRARIES = libreducer.a
libreducer_a_SOURCES = reducer.cpp 

libreducer_a_CPPFLAGS = -I$(parse_dir) \
					    -I$(model_dir) \
						-I$(putils_dir) \
						$(BOOST_CPPFLAGS) \
						$(BOOST_LDFLAGS)
