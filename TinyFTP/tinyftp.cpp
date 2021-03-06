#include "common.h"
#include "tinyftp.h"
#include "tabwidget.h"
#include "localdirwidget.h"
#include "remotedirwidget.h"
#include "queuewidget.h"

TinyFTP::TinyFTP(QWidget *parent)
	: QMainWindow(parent)
{
	hSplitter = new QSplitter(Qt::Horizontal, this);
	/*vSplitter = new QSplitter(Qt::Vertical, this);*/
	
	userNameLabel = new QLabel(tr("用户:"), this);
	userNameComboBox = new QComboBox(this);
	userNameComboBox->setEditable(true);
    userNameComboBox->setFixedWidth(100);

	passwordLabel = new QLabel(tr("口令:"), this);
	passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setFixedWidth(100);

	portLabel = new QLabel(tr("端口:"), this);
	portLineEdit = new QLineEdit(tr("21"), this);
    portLineEdit->setFixedWidth(50);
    portLineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	anonymousCheckBox = new QCheckBox(tr("匿名"), this);

	addressLabel = new QLabel(tr("地址:"), this);

	addressComboBox = new QComboBox(this);
	addressComboBox->setEditable(true);
    addressComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	goPushButton = new QPushButton(tr("转到"), this);

	QToolBar *userInfoToolBar = addToolBar(tr("用户信息"));
	userInfoToolBar->addWidget(userNameLabel);
	userInfoToolBar->addWidget(userNameComboBox);
    userInfoToolBar->addSeparator();
	userInfoToolBar->addWidget(passwordLabel);
	userInfoToolBar->addWidget(passwordLineEdit);
    userInfoToolBar->addSeparator();
	userInfoToolBar->addWidget(portLabel);
	userInfoToolBar->addWidget(portLineEdit);
    userInfoToolBar->addSeparator();
	userInfoToolBar->addWidget(anonymousCheckBox);
	userInfoToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(addressLabel);
    splitter->addWidget(addressComboBox);
    splitter->addWidget(goPushButton);
    splitter->setStretchFactor(1,1);
	QToolBar *addressInfoToolBar = addToolBar(tr("地址信息"));
    addressInfoToolBar->addWidget(splitter);
	addressInfoToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	localDirTabWidget = new TabWidget(this);
    localDirTabWidget->setEnableMutiTab(false);
	localDirTabWidget->addTab(new LocalDirWidget(this), tr("本地"));

	remoteDirTabWidget = new TabWidget(this);
	remoteDirTabWidget->setEnableMutiTab(true);
    remoteDirTabWidget->addTab(new RemoteDirWidget(this), tr(" "));

	ftpStatusBar = statusBar();

	queueWidget = new QueueWidget(tr("队列"), this);
	
	/*queueWidget->setFloatable(false);*/
	//addToolBar(Qt::BottomToolBarArea, queueWidget);
	addDockWidget(Qt::BottomDockWidgetArea, queueWidget);

	hSplitter->addWidget(localDirTabWidget);
	hSplitter->addWidget(remoteDirTabWidget);
// 	vSplitter->addWidget(hSplitter);
// 	vSplitter->addWidget(queueWidget);
	setCentralWidget(hSplitter);

	readSettings();

	connect(goPushButton, SIGNAL(clicked()), 
		this, SLOT(connectToFTPServer()));
	connect(anonymousCheckBox, SIGNAL(stateChanged(int)), 
		this, SLOT(anonymous(int)));
	connect(userNameComboBox, SIGNAL(currentIndexChanged(const QString &)), 
		this, SLOT(currentUsernameChanged(const QString &)));

	userNameComboBox->addItems(userNamePasswordMap.keys());
    addressComboBox->addItems(addressList);

	setWindowTitle(tr("TinyFTP"));
}

TinyFTP::~TinyFTP()
{

}

RemoteDirWidget * TinyFTP::remoteCurrentWidget() const
{
	return static_cast<RemoteDirWidget *>(remoteDirTabWidget->currentWidget());
}

LocalDirWidget * TinyFTP::localCurrentWidget() const
{
	return static_cast<LocalDirWidget *>(localDirTabWidget->currentWidget());
}

void TinyFTP::writeSettings()
{
	QSettings settings(tr("MyQt4Projects"), tr("TinyFTP"));
	settings.setValue("geometry", saveGeometry());
	QStringList usrnameList = userNamePasswordMap.keys();
	QStringList passwdList = userNamePasswordMap.values();
	settings.setValue("username", usrnameList);
	settings.setValue("password", passwdList);
    settings.setValue("addressList", addressList);
}

void TinyFTP::readSettings()
{
	QSettings settings(tr("MyQt4Projects"), tr("TinyFTP"));
    restoreGeometry(settings.value("geometry").toByteArray());
	QStringList usrnameList = settings.value("username").toStringList();
	QStringList passwdList = settings.value("password").toStringList();
	for (int i = 0; i < usrnameList.count(); i++) {
		userNamePasswordMap[usrnameList[i]] = passwdList[i];
	}
    addressList = settings.value("addressList").toStringList();
}

void TinyFTP::closeEvent(QCloseEvent *event)
{
	writeSettings();
}

void TinyFTP::connectToFTPServer()
{
    if (!okToConnectToFTPServer()) {
        return ;
    }

    QString port = portLineEdit->text();
    QString address = trimUrl(addressComboBox->currentText());
    QString username = userNameComboBox->currentText();
    QString password = passwordLineEdit->text();
    if (addressComboBox->findText(address) == -1) {
        addressComboBox->addItem(address);
        addressList.append(address);
    }
    addressComboBox->setEditText(address);

    RemoteDirWidget *remoteDirWidget = qobject_cast<RemoteDirWidget*>(remoteDirTabWidget->currentWidget());
    if (anonymousCheckBox->isChecked()) {
        username = QString();
        password = QString();
    } else {
		if (userNameComboBox->findText(username) == -1) {
			userNameComboBox->addItem(username);
		}
		userNamePasswordMap[username] = password;
	}
    remoteDirTabWidget->setTabText(remoteDirTabWidget->currentIndex(), address);
    remoteDirWidget->connectToHost(address, port, username, password);
}

void TinyFTP::ftpCommandDone(QFtp::Command command, bool error)
{
	RemoteDirWidget *remoteDirWidget = static_cast<RemoteDirWidget*>(sender());
	if (command == QFtp::ConnectToHost) {
		if (!error) {
		} else {
		}
	} else if (command == QFtp::Login) {
		if (!error) {
		} 
		else {
		}
	} else if (command == QFtp::Close) {
	} else if (command == QFtp::List) {
	} else if (command == QFtp::Cd) {
	} else if (command == QFtp::Get) {
	} else if (command == QFtp::Put) {
	} else if (command == QFtp::Remove) {
	} else if (command == QFtp::Mkdir) {
	} else if (command == QFtp::Rmdir) {
	} else if (command == QFtp::Rename) {
	}
}

bool TinyFTP::okToConnectToFTPServer()
{
    return true;
}

void TinyFTP::anonymous(int state)
{
    if (state == Qt::Checked) {
        userNameComboBox->setEnabled(false);
        passwordLineEdit->setEnabled(false);
    } else {
        userNameComboBox->setEnabled(true);
        passwordLineEdit->setEnabled(true);
    }
}

void TinyFTP::currentUsernameChanged(const QString &text)
{
	if (userNamePasswordMap.count(text)) {
		passwordLineEdit->setText(userNamePasswordMap[text]);
	}
}

QString TinyFTP::trimUrl(const QString &url)
{
    QString u = QDir::fromNativeSeparators(url);
    QString t = tr("ftp://");
    if (u.startsWith(t)) {
        u = u.mid(t.length());
    }
    return u;
}

void TinyFTP::resizeEvent(QResizeEvent *event)
{
    
}
