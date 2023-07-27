SRC = BadLogin.h BadLogin.cpp BadPassword.h BadPassword.cpp User.h User.cpp Message.h Message.cpp Chat.h Chat.cpp Main.cpp DataBase.h DataBase.cpp Logger.h Logger.cpp

chat : $(SRC)
	g++ -std=c++17 -o chat $(SRC)

install:
	install ./chat /usr/local/bin

uninstall:
	rm - rf /usr/local/bin/chat
