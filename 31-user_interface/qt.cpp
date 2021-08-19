/**
 * bardzo prosty przykład GUI w Qt
 * 
 * kompilacja: g++ `pkg-config --cflags Qt5Widgets` `pkg-config --libs Qt5Widgets` -fPIC qt.cpp
 * (wymaga pakietu: qtbase5-dev)
 *
 * analogiczny program w Pythonie znajdziesz na: http://vip.opcode.eu.org/#Graficzny_interfejs_użytkownika
 */

#include <qapplication.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

void handleButton() {
	int response = QMessageBox::question(NULL, "Message",
		"Tak czy nie?", QMessageBox::Yes |
		QMessageBox::No, QMessageBox::No);
	if (response == QMessageBox::Yes) {
		puts("YES");
	} else if (response == QMessageBox::No) {
		puts("NO");
	} else {
		puts("cos innego?!");
	}
}

int main( int argc, char **argv ) {
	QApplication app( argc, argv );
	
	QWidget win;
	win.resize(250, 150);
	win.move(300, 300);
	win.setWindowTitle("ABC ...");
	
	QPushButton button("Nacisnij mnie", &win);
	QObject::connect(&button, &QPushButton::clicked, handleButton);
	button.resize(button.sizeHint());
	button.move(
		250/2-button.sizeHint().width()/2,
		150/2-button.sizeHint().height()/2
	);
	
	win.show();
	return app.exec();
}
