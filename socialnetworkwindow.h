#ifndef SOCIALNETWORKWINDOW_H
#define SOCIALNETWORKWINDOW_H

#include <QMainWindow>

#include "../network.h"
#include "../user.h"
#include "../post.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class SocialNetworkWindow;
}
QT_END_NAMESPACE

class SocialNetworkWindow : public QMainWindow
{
    Q_OBJECT

public:
    SocialNetworkWindow(QWidget *parent = nullptr);
    ~SocialNetworkWindow();

    // display functions
    void display();
    void tabsVisible(bool, bool, bool, bool);
    void tabsSync();
    void populateSelfFriends();
    void populateOtherFriends(User*);
    void populateSuggestions(User*);
    void populateSelfPosts();
    void populateOtherPosts(User*, bool);
    void populateSelfPoll(int);
    void populateOtherPoll(int);
    void updateCreatePollTable();
    void voteCast(int row, int column);

    // slots (user actions)
    void loginAttempt();
    void changeTab(int);
    void toFriendProfile(int row, int column);
    void toSuggestedProfile(int row, int column);
    void toOtherFriendProfile(int row, int column);
    void addFriendButton();
    void addFriendTable(int row, int column);
    void addPost();
    void addPollOption();
    void removePollOption();
    void addPoll();
    void updateSelfPollTable(int);
    void updateOtherPollTable(int);

private:
    Ui::SocialNetworkWindow *ui;

    Network net;
    User* self_user;
    User* other_user;
    // page states
    bool logged_in;
    int tab;
    Poll* self_poll;
    Poll* other_poll;

};
#endif // SOCIALNETWORKWINDOW_H
