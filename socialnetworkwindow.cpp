#include "socialnetworkwindow.h"
#include "ui_socialnetworkwindow.h"


#include <iostream>


SocialNetworkWindow::SocialNetworkWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SocialNetworkWindow)
{
    ui->setupUi(this);
    // initialize ui states
    logged_in = false;
    self_user = other_user = nullptr;
    self_poll = other_poll = nullptr;
    tab = 0;
    tabsVisible(true, false, false, false);
    // initialize social network
    net = Network();
    net.readUsers("users.txt");
    net.readPosts("posts.txt");
    net.readPolls("polls.txt");
    // connect ui elements to slot functions
    connect(ui->loginPushButton, &QPushButton::clicked, this, &SocialNetworkWindow::loginAttempt);
    connect(ui->tabs, &QTabWidget::currentChanged, this, &SocialNetworkWindow::changeTab);
    connect(ui->otherAddButton, &QPushButton::clicked, this, &SocialNetworkWindow::addFriendButton);
    connect(ui->profileFriendsTable, &QTableWidget::cellClicked, this, &SocialNetworkWindow::toFriendProfile);
    connect(ui->profileSuggestionsTable, &QTableWidget::cellClicked, this, &SocialNetworkWindow::toSuggestedProfile);
    connect(ui->otherFriendsTable, &QTableWidget::cellClicked, this, &SocialNetworkWindow::toOtherFriendProfile);
    //    post slots
    connect(ui->postButton, &QPushButton::clicked, this, &SocialNetworkWindow::addPost);
    connect(ui->pollMessageEdit, &QTextEdit::textChanged, this, &SocialNetworkWindow::updateCreatePollTable);
    connect(ui->pollAddOptionButton, &QPushButton::clicked, this, &SocialNetworkWindow::addPollOption);
    connect(ui->pollRemoveOptionButton, &QPushButton::clicked, this, &SocialNetworkWindow::removePollOption);
    connect(ui->pollButton, &QPushButton::clicked, this, &SocialNetworkWindow::addPoll);
    connect(ui->otherPollTable, &QTableWidget::cellClicked, this, &SocialNetworkWindow::voteCast);
    connect(ui->otherPollSpinBox, &QSpinBox::valueChanged, this, &SocialNetworkWindow::updateOtherPollTable);
    connect(ui->profilePollSpinBox, &QSpinBox::valueChanged, this, &SocialNetworkWindow::updateSelfPollTable);

    // update display
    SocialNetworkWindow::display();
}

SocialNetworkWindow::~SocialNetworkWindow()
{
    delete ui;
}

// Master Display function (shows, hides & updates widgets)
void SocialNetworkWindow::display() {
    // checks page variable states and displays proper widgets
    if (tab == 0) {  // login
        logged_in = false;
        tabsVisible(true, false, false, false);
        ui->tabs->setCurrentIndex(0);
    } else if (tab == 1) {  // profile
        tabsVisible(true, true, true, true);
        // prepare ui
        populateSelfFriends();
        populateSuggestions(self_user);
        populateSelfPosts();
        populateSelfPoll(-1);  // make function handle which poll
        ui->tabs->setCurrentIndex(1);
    } else if (tab == 2) {  // other
        // assign other_user if null
        if (other_user == nullptr) {
            // default to user's first friend
            int friend_1 = *(self_user->getFriends().begin());
            other_user = net.getUser(friend_1);     // if no friends, still nullptr
        }
        // prepare ui
        ui->otherAddButton->hide();
        if (other_user) {
            std::string label = other_user->getName() + "'s Profile";
            ui->otherTitleLabel->setText(QString::fromStdString(label));
            populateOtherFriends(other_user);
            populateOtherPosts(other_user, true);  // show only public
            populateOtherPoll(-1);  // make function handle which poll
            if (!net.areFriends(self_user->getId(), other_user->getId())) {
                ui->otherAddButton->show();
            }
        }
        tabsVisible(true, true, true, true);
        ui->tabs->setCurrentIndex(2);
    } else if (tab == 3) {  // post
        tabsVisible(true, true, true, true);
        ui->tabs->setCurrentIndex(3);
        ui->postEdit->document()->clear();  // reset Post TextEdit
        ui->postEdit->setPlaceholderText(QString::fromStdString("Write a post..."));
    }
}

// --- display() HELPER FUNCTIONS

void SocialNetworkWindow::tabsVisible(bool t0, bool t1, bool t2, bool t3) {
    ui->tabs->setTabVisible(0, t0);
    ui->tabs->setTabVisible(1, t1);
    ui->tabs->setTabVisible(2, t2);
    ui->tabs->setTabVisible(3, t3);
}

void SocialNetworkWindow::populateSelfFriends() {
    QStringList header = {"Friends"};
    ui->profileFriendsTable->setHorizontalHeaderLabels(header);
    std::set<int> friends = self_user->getFriends();
    int num_friends = friends.size();
    ui->profileFriendsTable->setRowCount(num_friends);
    int row = 0;
    for (int id : friends) {
        QString friend_name = QString::fromStdString(net.getUser(id)->getName());
        ui->profileFriendsTable->setItem(row, 0, new QTableWidgetItem(friend_name));
        row++;
    }
}

void SocialNetworkWindow::populateOtherFriends(User* user) {
    QStringList header = {"Friends"};
    ui->otherFriendsTable->setHorizontalHeaderLabels(header);
    std::set<int> friends = user->getFriends();
    int num_friends = friends.size();
    ui->otherFriendsTable->setRowCount(num_friends);
    int row = 0;
    for (int id : friends) {
        QString friend_name = QString::fromStdString(net.getUser(id)->getName());
        ui->otherFriendsTable->setItem(row, 0, new QTableWidgetItem(friend_name));
        row++;
    }
}

void SocialNetworkWindow::populateSelfPosts() {
    std::string posts = self_user->getPostsString(5, false);
    ui->profilePostsLabel->setText(QString::fromStdString(posts));
    ui->profilePostsLabel->adjustSize();
    // UNFINISHED - use table for posts instead
/*
    std::vector<Post*> posts = self_user->getPosts();
    ui->profilePostsTable->setRowCount(posts.size());
    int row = 0;
    for (Post* p : posts) {
        QString message = QString::fromStdString(p->toString());
        ui->profilePostsTable->setItem(row, 0, new QTableWidgetItem(message));
        row++;
    }
*/
}

void SocialNetworkWindow::populateOtherPosts(User* user, bool onlyPublic) {
    std::string posts = user->getPostsString(5, onlyPublic);
    ui->otherPostsLabel->setText(QString::fromStdString(posts));
    ui->otherPostsLabel->adjustSize();
}

void SocialNetworkWindow::populateSuggestions(User* self_user) {
    QStringList header = {"Suggested Friends"};
    ui->profileSuggestionsTable->setHorizontalHeaderLabels(header);
    int score;
    std::vector<int> suggestions = net.suggestFriends(self_user->getId(), score);
    ui->profileSuggestionsTable->setRowCount(suggestions.size());
    int row = 0;
    for (int id : suggestions) {
        QString suggestion_name = QString::fromStdString(net.getUser(id)->getName());
        ui->profileSuggestionsTable->setItem(row, 0, new QTableWidgetItem(suggestion_name));
        row++;
    }
}

void SocialNetworkWindow::populateSelfPoll(int messageId) {
    int id = messageId;
    if (self_user->getPolls().empty()) {
        ui->profilePollTable->clear();
        ui->profilePollSpinBox->setMaximum(0);
        return;
    } else if (messageId == -1) {
        id = self_user->getPolls()[0]->getMessageId();
    }
    Poll* poll = self_user->getPollId(id);
    self_poll = poll;
    ui->profilePollSpinBox->setMaximum(self_user->getPolls().size()-1);
    QStringList header = {QString::fromStdString(poll->getMessage())};
    ui->profilePollTable->setHorizontalHeaderLabels(header);
    ui->profilePollTable->setRowCount(poll->numOptions());
    int row = 0;
    for (PollOption* opt : poll->getOptions()) {
        QString option = QString::fromStdString(opt->toString(poll->getTotalVotes()));
        ui->profilePollTable->setItem(row, 0, new QTableWidgetItem(option));
        row++;
    }
    ui->profilePollTable->setItem(row, 0, new QTableWidgetItem(poll->getMessageId()));
}

// same as above, just different ui elements
void SocialNetworkWindow::populateOtherPoll(int messageId) {
    int id = messageId;
    if (other_user->getPolls().empty()) {
        ui->otherPollTable->clear();
        ui->otherPollSpinBox->setMaximum(0);
        return;
    } else if (messageId == -1) {
        id = other_user->getPolls()[0]->getMessageId();
    }
    Poll* poll = other_user->getPollId(id);
    other_poll = poll;
    ui->otherPollSpinBox->setMaximum(other_user->getPolls().size()-1);
    QStringList header = {QString::fromStdString(poll->getMessage())};
    ui->otherPollTable->setHorizontalHeaderLabels(header);
    ui->otherPollTable->setRowCount(poll->numOptions());
    int row = 0;
    for (PollOption* opt : poll->getOptions()) {
        QString option = QString::fromStdString(opt->toString(poll->getTotalVotes()));
        ui->otherPollTable->setItem(row, 0, new QTableWidgetItem(option));
        row++;
    }
    ui->otherPollTable->setItem(row, 0, new QTableWidgetItem(poll->getMessageId()));
}

void SocialNetworkWindow::updateCreatePollTable() {
    QStringList header = {ui->pollMessageEdit->toPlainText()};
    ui->pollTable->setHorizontalHeaderLabels(header);
}




// -- SLOT FUNCTIONS (update variables, call display)

void SocialNetworkWindow::changeTab(int index) {
    tab = index;
    display();
}

void SocialNetworkWindow::toFriendProfile(int row, int column) {
    if (ui->profileFriendsTable->item(row, column)) {  // only if valid item clicked
        std::string name = (ui->profileFriendsTable->item(row, column)->text()).toStdString();
        User *target = net.getUser(net.getId(name));
        // check if the target is My Profile
        if (target == self_user) {
            tab = 1;
        } else {
            other_user = target;
            tab = 2;
        }
        display();
    }
}

void SocialNetworkWindow::toSuggestedProfile(int row, int column) {
    if (ui->profileSuggestionsTable->item(row, column)) {  // only if valid item clicked
        std::string name = (ui->profileSuggestionsTable->item(row, column)->text()).toStdString();
        other_user = net.getUser(net.getId(name));
        tab = 2;
        display();
    }
}

void SocialNetworkWindow::toOtherFriendProfile(int row, int column) {
    if (ui->otherFriendsTable->item(row, column)) {  // only if valid item clicked
        std::string name = (ui->otherFriendsTable->item(row, column)->text()).toStdString();
        User *target = net.getUser(net.getId(name));
        // check if the target is My Profile
        if (target == self_user) {
            tab = 1;
        } else {
            other_user = target;
            tab = 2;
        }
        display();
    }
}

void SocialNetworkWindow::loginAttempt() {
    logged_in = false;
    // check if valid user name
    std::string name_input = ui->loginTextEdit->toPlainText().toStdString();
    int id_input = net.getId(name_input);
    if (id_input != -1) {
        logged_in = true;
        self_user = net.getUser(id_input);
        tab = 1;
        ui->loginTextEdit->setText("");    // clear text for next login
        display();
    } else {
        ui->loginLabel->setText("Please enter a valid user name");
    }
}

void SocialNetworkWindow::addFriendButton() {
    net.addConnection(self_user->getName(), other_user->getName());
    net.writeUsers("users.txt");
    display();
}

void SocialNetworkWindow::addPost() {
    std::string message = ui->postEdit->toPlainText().toStdString();
    if (message != "") {
        if (!ui->postIncoming->isChecked()) {
            net.addPost(self_user->getId(), message, 0, false, self_user->getName(), true);
        } else {
            std::string name_input = ui->postIncomingEdit->toPlainText().toStdString();
            int id_input = net.getId(name_input);
            if (id_input != -1) {
                net.addPost(id_input, message, 0, true, self_user->getName(), true);
            } else {
                ui->postIncomingEdit->clear();
                ui->postIncomingEdit->setPlaceholderText("Enter a valid user!");
            }
        }
        net.writePosts("posts.txt");
        display();
    }
}

//    Poll Slots

void SocialNetworkWindow::addPollOption() {
    int rows = ui->pollTable->rowCount();
    ui->pollTable->setRowCount(rows+1);
    QString opt_name = ui->pollOptionEdit->toPlainText();
    ui->pollTable->setItem(rows, 0, new QTableWidgetItem(opt_name));
    ui->pollOptionEdit->clear();
}

void SocialNetworkWindow::removePollOption() {
    int rows = ui->pollTable->rowCount();
    ui->pollTable->removeCellWidget(rows-1, 0);
    ui->pollTable->setRowCount(rows-1);
}

void SocialNetworkWindow::addPoll() {
    int ownerId = self_user->getId();
    int totalVotes = 0;
    std::string message = ui->pollMessageEdit->toPlainText().toStdString();
    std::vector<PollOption*> options;
    for (int i=0; i<ui->pollTable->rowCount(); i++) {
        auto cell = ui->pollTable->item(i, 0);
        std::string name = cell->text().toStdString();
        options.push_back(new PollOption(name));
    }
    net.addPoll(ownerId, message, options, totalVotes);
    net.writePolls("polls.txt");
    // clear text edits
    ui->pollMessageEdit->clear();
    ui->pollOptionEdit->clear();
    ui->pollTable->clear();
}

void SocialNetworkWindow::voteCast(int row, int column) {
    int messageId = other_poll->getMessageId();
    net.addVote(other_user->getId(), messageId, self_user->getId(), row);
    net.writePolls("polls.txt");
    populateOtherPoll(messageId);
}

void SocialNetworkWindow::updateSelfPollTable(int poll_num) {
    // determine if valid poll #
    if (poll_num < self_user->getPolls().size()) {
        int messageId = self_user->getPolls()[poll_num]->getMessageId();
        populateSelfPoll(messageId);
    }
}

// same as above, different calls
void SocialNetworkWindow::updateOtherPollTable(int poll_num) {
    // determine if valid poll #
    if (poll_num < other_user->getPolls().size()) {
        int messageId = other_user->getPolls()[poll_num]->getMessageId();
        populateOtherPoll(messageId);
    }
}
