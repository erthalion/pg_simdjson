MODULE_big = pg_simdjson
OBJS = pg_simdjson.o

PG_CPPFLAGS += -O3 -std=c++17 -I$(includedir_server) -fPIC -fno-rtti
LDFLAGS=-fno-rtti

pg_simdjson.o: pg_simdjson.cpp
	$(CXX) $(PG_CPPFLAGS) -c pg_simdjson.cpp
	$(CXX) $(LDFLAGS) -shared -o pg_simdjson.so pg_simdjson.o

EXTENSION = pg_simdjson
DATA = pg_simdjson--0.1.sql
PGFILEDESC = "pg_simdjson"

HEADERS = pg_simdjson.h

REGRESS = pg_simdjson

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
