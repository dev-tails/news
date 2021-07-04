release:
	g++ -o api api.cpp -lpthread -lboost_system -lsqlite3

debug:
	g++ -o api api.cpp -ggdb -lpthread -lboost_system -lsqlite3 -DCROW_ENABLE_DEBUG

ssl:
	g++ -o api api.cpp -lssl -lcrypto -lpthread -lboost_system -lsqlite3 -DCROW_ENABLE_SSL