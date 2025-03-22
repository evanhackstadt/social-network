// Evan Hackstadt 2025 Winter CSCI 62

#include <string>
#include <set>

#include "user.h"


// CONSTRUCTORS

User::User() {
	id_ = -1;
	name_ = "";
	year_ = -1;
	zip_ = -1;
	friends_ = std::set<int>();
	messages_ = std::vector<Post*>();
}

User::User(int id, std::string name, int year, int zip, std::set<int> friends) {
	id_ = id;
	name_ = name;
	year_ = year;
	zip_ = zip;
	friends_ = friends;
	messages_ = std::vector<Post*>();
}

// ACCESSORS

int User::getId() const { return id_; }

std::string User::getName() const { return name_; }

int User::getYear() const { return year_; }

int User::getZip() const { return zip_; }

std::set<int> & User::getFriends() { return friends_; }

std::vector<Post*> User::getPosts() { return messages_; }

std::vector<Poll*> User::getPolls() { return polls_; }

Post* User::getPostId(int messageId) {
	for (auto post : messages_) {
		if (post->getMessageId() == messageId) {
			return post;
		}
	}
	return nullptr;
}

Poll* User::getPollId(int messageId) {
	for (auto poll : polls_) {
		if (poll->getMessageId() == messageId) {
			return poll;
		}
	}
	return nullptr;
}



std::string User::getPostsString(int howMany, bool showOnlyPublic) {
	std::string output;
	std::vector<Post*> posts = getPosts();
	int size = posts.size();
	int toDisplay = (howMany > size ? size : howMany);
	for (int i=0; i<toDisplay; i++) {
		if (!showOnlyPublic || (showOnlyPublic && posts[size-1-i]->getIsPublic())) {
			output += posts[size-1-i]->toString();
			output += "\n\n";
		}
	}
	return output;
}



// MUTATORS

void User::addFriend(int id) {
	if (getFriends().find(id) != getFriends().end()) {
		return;		// if id already added
	} else {
		getFriends().insert(id);
	}
}

void User::deleteFriend(int id) {
	if (getFriends().find(id) == getFriends().end()) {
		return;		// if id not present
	} else {
		getFriends().erase(id);
	}
}

void User::addPost(Post* p) {
	messages_.push_back(p);
}

void User::addPoll(Poll* p) {
	polls_.push_back(p);
}