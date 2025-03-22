// Evan Hackstadt 2025 Winter CSCI 62

#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <vector>

#include "user.h"
#include "post.h"


class Network {

public:
	// default constructor
	// pre: none
	// post: initializes an empty network with no users
	Network();


	// ACCESSORS

	// pre: none
	// post: returns a pointer to the user object with given ID.
	//       if no matching user, returns nullptr.
	User* getUser(int id);

	// pre: given a string in format "firstname lastname"
	// post: returns ID of user with given name.
	//       if no matching user, returns -1.
	int getId(std::string name);

	// pre: none
	// post: returns the number of Users in the network
	int numUsers();

	// Graph Analysis Accessors

	// pre: from and to are each valid User IDs in the network
	// post: returns vector containing the IDs that, when followed,
	//       give the shortest path between from and to
	std::vector<int> shortestPath(int from, int to);

	// pre: from and to are valid User IDs in the network
	// post: finds first User at given distance; sets "to" to this User's ID
	//       and returns vector of the path of given distance to this User
	std::vector<int> distanceUser(int from, int& to, int distance);

	// pre: given two valid User IDs in the network
	// post: returns true if the Users are friends, and false otherwise.
	// helper function for suggestFriends()
	bool areFriends(int user1, int user2);

	// pre: given a valid User ID who
	// post: returns a vector of IDs of suggested friends,
	//       based on potential mutual friends,
	//       and sets score to the highest mutual friends this User could have
	//       with a new friend
	std::vector<int> suggestFriends(int who, int& score);

	// pre: none
	// post: returns a list containing groups of Users (that are all connected
	//       to each other, but not to any other Users)
	std::vector<std::vector<int> > groups();

	// Posts Accessor

	// pre: given a valid ownerId in the Network
	// post: calls getPostsString on the specified User
	std::string getPostsString(int ownerId, int howMany, bool showOnlyPublic);


	// MUTATORS

	// pre: given a pointer to a User object
	// post: adds given User to the network (users_)
	void addUser(User*);

	// pre: passed in two strings, each in "firstname lastname" format
	// post: creates friend connection between the two Users and returns 0.
	//       if either User is invalid, returns -1.
	int addConnection(std::string s1, std::string s2);

	// pre: passed in two strings, each in "firstname lastname" format
	// post: removes friend connection between the two Users and returns 0.
	//       if either User is invalid, returns -1.
	int deleteConnection(std::string s1, std::string s2);

	// Helper function for addPost and readPosts
	// pre: given a valid ownerId in the Network
	// post: adds a post to the specified user, with a specified messageId
	void addIdPost(int messageId, int ownerId, std::string message, int likes,
		bool isIncoming, std::string authorName, bool isPublic);

	// Helper function for addPost
	// pre: none
	// post: returns the proper messageId (current highest id + 1)
	int findMessageId();

	// pre: given a valid ownerId in the Network
	// post: adds the post to the specified user
	void addPost(int ownerId, std::string message, int likes,
		bool isIncoming, std::string authorName, bool isPublic);

	// Poll Post Mutators

	// Helper function for addPoll
	int findPollMessageId();

	int findPollMessageIdName(std::string name);

	// pre: given valid ownerId in the Network
	// post adds the poll post to the specified user
	void addPoll(int ownerId, std::string message,
		std::vector<PollOption*> options, int totalVotes);

	void addIdPoll(int messageId, int ownerId, std::string message,
		std::vector<PollOption*> options, int totalVotes);

	// pre: post with messageId exists and is a Poll Post,
	//      voter and owner are valid users, and poll has option
	// post: adds 1 to the option of the poll,
	//       adds the user's id to voters_ (delegates)
	void addVote(int ownerId, int messageId, int voterId, int option);

	// pre: post with messageId exists and is a Poll Post,
	//      voter and owner are valid users, and poll has option
	// post: removes 1 from the option of the poll,
	//       removes the user's id from voters_ (delegates)
	void removeVote(int ownerId, int messageId, int voterId, int option);




	// FILE IO

	// pre: a well-formatted file of users
	// post: initializes all network info (users with info and friends)
	void readUsers(const char* fname);

	// pre: none
	// post: writes all network info (users with info and friends)
	//       into a well-formatted file of users
	void writeUsers(const char* fname);

	// pre: none; for intended function: well formatted file fname
	// post: adds all data from file into posts in the Network,
	//       returning number of posts read or -1 if file cannot be read
	int readPosts(char* fname);

	// pre: none
	// post: writes all data from Network's posts into the file
	//       in order of increasing messageId,
	//       returning number of posts written or -1 if file cannot be read
	int writePosts(char* fname);

	// pre: none
	// post:
	int readPolls(char* fname);

	// pre:
	// post:
	int writePolls(char* fname);


private:
	std::vector<User*> users_;

	// INVARIANTS:
	// users_[i] points to a dynamically-allocated user object with ID i
	// for all i=0,1,2,...users_.size()-1
};

#endif    // NETWORK_H