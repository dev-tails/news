release:
	g++ -o api api.cpp -lpthread -lboost_system

debug:
	g++ -o api api.cpp -ggdb -lpthread -lboost_system -DCROW_ENABLE_DEBUG

ssl:
	g++ -o api api.cpp -lssl -lcrypto -lpthread -lboost_system -DCROW_ENABLE_SSL