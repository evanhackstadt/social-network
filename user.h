// Evan Hackstadt 2025 Winter CSCI 62

#ifndef USER_H
#define USER_H

#include <string>
#include <set>

#include "post.h"


class User {

public:

	// CONSTRUCTORS

	// default constructor
	// pre: none
	// post: initializes User class with arbitrary values
	User();

	// primary constructor
	// pre: none
	// post: initializes User class with parameter values
	User(int id, std::string name, int year, int zip, std::set<int> friends);

	// ACCESSORS

	// pre: none
	// post: returns User's id
	int getId() const;

	// pre: none
	// post: returns User's name
	std::string getName() const;

	// pre: none
	// post: returns User's year of birth
	int getYear() const;

	// pre: none
	// post: returns User's zip code
	int getZip() const;

	// pre: none
	// post: returns reference to User's friends
	std::set<int> & getFriends();

	// pre: none
	// post: returns messages_
	std::vector<Post*> getPosts();

	// pre: none
	// post: returns polls_
	std::vector<Poll*> getPolls();

	// pre: given an existing message ID to a post
	// post: returns pointer to the user's post with that ID
	Post* getPostId(int messageId);

	// pre: given an existing message ID to a poll
	// post: returns a pointer to the user's poll with that ID
	Poll* getPollId(int messageId);

	// pre: messages_ is not empty
	// post: returns a string of howMany recent posts by the User
	//       (either all, or only public, as indicated)
	//       concatenated and separated by two newlines.
	std::string getPostsString(int howMany, bool showOnlyPublic);

	// MUTATORS

	// pre: user object initialized
	// post: adds the id to the user's friend set
	//       unless it was already present
	void addFriend(int id);

	// pre: user object initialized
	// post: removes the id from the user's friend set
	//       unless it wasn't present
	void deleteFriend(int id);

	// pre: none
	// post: pushes back a pointer to a Post to this User's messages_
	void addPost(Post*);

	// pre: none
	// post: pushes back a pointer to a Poll to this User's polls_
	void addPoll(Poll*);


private:
	int id_;
	std::string name_;
	int year_;
	int zip_;
	std::set<int> friends_;
	std::vector<Post*> messages_;
	std::vector<Poll*> polls_;

	// INVARIANTS:
	// id_ is 0,1,2... according to index in Network's user list
	// name_ is "first last"

};

#endif    // USER_H