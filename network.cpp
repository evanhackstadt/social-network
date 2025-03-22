#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "network.h"


// --CONSTRUCTOR
Network::Network() {
	users_ = std::vector<User*>();
}

// --ACCESSORS

User* Network::getUser(int id) {
	return users_[id];
}

int Network::getId(std::string name) {
	for (int id=0; id<numUsers(); id++) {
		if (users_[id]->getName() == name) {
			return id;
		}
	}
	return -1;
}

int Network::numUsers() {
	return users_.size();
}

std::string Network::getPostsString(int ownerId, int howMany, bool showOnlyPublic) {
	return getUser(ownerId)->getPostsString(howMany, showOnlyPublic);
}

// Graph Analysis Accessors

std::vector<int> Network::shortestPath(int from, int to) {
	// use BFS to find shortest path in graph
	std::vector<int> output;
	std::vector<int> prev(numUsers(), -1);
	std::vector<bool> visited(numUsers(), 0);
	std::queue<int> todo;

	visited[from] = true;
	todo.push(from);

	while (!todo.empty()) {
		int cur = todo.front();
		todo.pop();
		for (int neighbor : getUser(cur)->getFriends()) {
			if (!visited[neighbor]) {
				prev[neighbor] = cur;
				todo.push(neighbor);
				visited[neighbor] = true;
			}
		}
	}
	// backtrack path using prev
	int cur = to;
	while (cur != -1) {
		output.push_back(cur);
		cur = prev[cur];
	}
	reverse(output.begin(), output.end());
	return output;
}

std::vector<int> Network::distanceUser(int from, int& to, int distance) {
	// use BFS but also track distance, return first node at distance
	std::vector<int> output;
	std::vector<int> prev(numUsers(), -1);
	std::vector<int> dist(numUsers(), -1);
	std::vector<bool> visited(numUsers(), 0);
	std::queue<int> todo;

	dist[from] = 0;
	visited[from] = true;
	todo.push(from);

	while(!todo.empty()) {
		int cur = todo.front();
		todo.pop();
		for (int neighbor : getUser(cur)->getFriends()) {
			if (!visited[neighbor]) {
				if (dist[cur]+1 == distance) {   // distance found
					prev[neighbor] = cur;
					to = neighbor;    // set reference variable
					// prepare output - backtrack & reverse
					int temp = to;
					while(temp != -1) {
						output.push_back(temp);
						temp = prev[temp];
					}
					reverse(output.begin(), output.end());
					return output;    // end function
				} else {
					dist[neighbor] = dist[cur] + 1;
					prev[neighbor] = cur;
					todo.push(neighbor);
					visited[neighbor] = true;
				}
			}
		}
	}
	/// exact distance not found - return error case
	to = -1;
	return output;
}

// helper function for suggestFriends()
bool Network::areFriends(int user1, int user2) {
	std::set<int> user1_friends = getUser(user1)->getFriends();
    if (user1_friends.count(user2) != 0) {
        return true;
	}
	return false;
}

std::vector<int> Network::suggestFriends(int who, int& score) {
	std::vector<int> output;
	std::vector<int> scores(numUsers(), 0);
	std::set<int> who_fr = getUser(who)->getFriends();

	for (User* cur_usr : users_) {
		int cur_id = cur_usr->getId();
		if (cur_id!=who && !areFriends(cur_id, who)) {    // ignore user & their friends
			std::set<int> cur_fr = cur_usr->getFriends();
			std::vector<int> mutual_fr(numUsers());    // pre-allocate
			std::vector<int>::iterator end_of_range;
			// find the mutual friends of user who and this current user
			end_of_range = std::set_intersection(who_fr.begin(), who_fr.end(),
				cur_fr.begin(), cur_fr.end(), mutual_fr.begin());
			// trim the pre-allocated vector to the end of intersect range
			mutual_fr.resize(end_of_range - mutual_fr.begin());
			scores[cur_id] = mutual_fr.size();
		}
	}

	score = *std::max_element(scores.begin(), scores.end());
	if (score == 0) {
		return output;
	} else {
		for (int i=0; i<scores.size(); i++) {
			if (scores[i] == score) {    // only push back IDs w/ max scores
				output.push_back(i);
			}
		}
		return output;
	}
}

// helper function for groups()
int findUnvisited(std::vector<bool> & visited) {
	for (int i=0; i<visited.size(); i++) {
		if (visited[i] == false) {
			return i;
		}
	}
	return -1;
}

std::vector<std::vector<int>> Network::groups() {
	std::vector<std::vector<int> > output;
	std::vector<bool> visited(numUsers(), false);
	
	int unvisited_id = findUnvisited(visited);

	while (unvisited_id != -1) {    // for each group
		std::vector<int> group;
		group.push_back(unvisited_id);
		visited[unvisited_id] = true;
		// use DFS to get the rest of the users in this group
		std::stack<int> todo;
		todo.push(unvisited_id);
		while (!todo.empty()) {
			int cur_usr = todo.top();
			todo.pop();
			for (int neighbor : getUser(unvisited_id)->getFriends()) {
				if (!visited[neighbor]) {
					group.push_back(neighbor);    // part of this group
					todo.push(neighbor);
					visited[neighbor] = true;
				}
			}
		}    // DFS ends, all users in group found, now add to ouput
		output.push_back(group);
		unvisited_id = findUnvisited(visited);    // find next unvisited user
	}

	return output;
}



// --MUTATORS

void Network::addUser(User* p) {
	users_.push_back(p);
}

int Network::addConnection(std::string s1, std::string s2) {
	int id1 = getId(s1);
	int id2 = getId(s2);
	if (id1 == -1 || id2 == -1) {	// if either user invalid
		return -1;
	} else {
		users_[id1]->addFriend(id2);
		users_[id2]->addFriend(id1);
		return 0;
	}
}

int Network::deleteConnection(std::string s1, std::string s2) {
	int id1 = getId(s1);
	int id2 = getId(s2);
	if (id1 == -1 || id2 == -1) {	// if either user invalid
		return -1;
	} else {
		users_[id1]->deleteFriend(id2);
		users_[id2]->deleteFriend(id1);
		return 0;
	}
}

// helper function for addPost and readPosts
// that adds a post to a user given a specific messageId
void Network::addIdPost(int messageId, int ownerId, std::string message, int likes, bool isIncoming, std::string authorName, bool isPublic) {
	User *u = getUser(ownerId);
	if (isIncoming) {
		IncomingPost *i = 
		new IncomingPost(messageId, ownerId, message, likes, isPublic, authorName);
		u->addPost(i);
	} else {
		Post *p = new Post(messageId, ownerId, message, likes);
		u->addPost(p);
	}
}

// helper function for addPost
int Network::findMessageId() {
	// find max messageId in network
	int messageId = -1;
	for (User* user : users_) {
		std::vector<Post*> posts = user->getPosts();
		if (posts.size() != 0) {
			int latestId = posts.back()->getMessageId();
			if (latestId > messageId) {
				messageId = latestId;
			}
		}
	}
	return messageId+1;    // id will be the next after the current max
}

void Network::addPost(int ownerId, std::string message, int likes,
		bool isIncoming, std::string authorName, bool isPublic) {
	// determine proper messageId
	int messageId = findMessageId();
	// now add Post to the specified User
	addIdPost(messageId, ownerId, message, likes, isIncoming, authorName, isPublic);
}

// Poll Post Mutators

// helper function for addPoll
int Network::findPollMessageId() {
	// find max messageId in network
	int messageId = -1;
	for (User* user : users_) {
		std::vector<Poll*> polls = user->getPolls();
		if (polls.size() != 0) {
			int latestId = polls.back()->getMessageId();
			if (latestId > messageId) {
				messageId = latestId;
			}
		}
	}
	return messageId+1;    // id will be the next after the current max
}

void Network::addPoll(int ownerId, std::string message,
		std::vector<PollOption*> options, int totalVotes) {
	// determine proper messageId
	int messageId = findPollMessageId();
	// now add Poll to the specified user
	addIdPoll(messageId, ownerId, message, options, totalVotes);
}

void Network::addIdPoll(int messageId, int ownerId, std::string message,
		std::vector<PollOption*> options, int totalVotes) {
	Poll *p = new Poll(messageId, ownerId, message, options, totalVotes);
	User *u = getUser(ownerId);
	u->addPoll(p);
	std::cout << "added poll with id " << p->getMessageId() << std::endl;
}

void Network::addVote(int ownerId, int messageId, int voterId, int option) {
	getUser(ownerId)->getPollId(messageId)->addVote(voterId, option);
}

void Network::removeVote(int ownerId, int messageId, int voterId, int option) {
	getUser(ownerId)->getPollId(messageId)->removeVote(voterId, option);
}


// --FILE IO

void Network::readUsers(const char* fname) {
	int count, val, id, year, zip;
	std::string mystring, name;
	std::set<int> friends;
	// open file and first line
	std::ifstream myfile(fname);
	getline(myfile, mystring);
	count = std::stoi(mystring);

	users_ = std::vector<User*>(count);

	while (getline(myfile, mystring)) {    // for each user
		id = std::stoi(mystring);
		// name
		std::getline(myfile, mystring);
		name = mystring.erase(0,1);    // erase the \t
		// year
		std::getline(myfile, mystring);
		year = std::stoi(mystring);
		// zip
		std::getline(myfile, mystring);
		zip = std::stoi(mystring);
		// friend IDs
		friends.clear();    // clear any ids from prev user
		std::getline(myfile, mystring);
		std::stringstream ss(mystring);
		while (ss >> val) {    // reads friend IDs until end
			friends.insert(val);
		}
		users_[id] = new User(id, name, year, zip, friends);
	}
}

void Network::writeUsers(const char* fname) {
	// write file
	std::ofstream fout(fname);
	// count
	fout << numUsers() << std::endl;
	for (int i=0; i<numUsers(); i++) {    // for each user
		fout << i << std::endl;    // id
		fout << "\t" << users_[i]->getName() << std::endl;
		fout << "\t" << users_[i]->getYear() << std::endl;
		fout << "\t" << users_[i]->getZip() << std::endl;
		// friends
		std::set<int> friends = users_[i]->getFriends();
		fout << "\t";
		for (std::set<int>::iterator it=friends.begin(); it!=friends.end(); it++) {
			fout << *it << " ";
		}
		fout << std::endl;
	}
	fout.close();
}

int Network::readPosts(char* fname) {
	int count, messageId, ownerId, likes;
	std::string instring, message, author;
	bool isPublic, isIncoming;
	// open file and get number of posts
	std::ifstream fin(fname);
	if (fin.bad()) return -1;
	getline(fin, instring);
	count = std::stoi(instring);

	while (getline(fin, instring)) {    // for each post
		messageId = std::stoi(instring);
		// message text
		getline(fin, message);
		message.erase(0,1);    // erase the \t
		// ownerId
		getline(fin, instring);
		ownerId = std::stoi(instring);
		// likes
		getline(fin, instring);
		likes = std::stoi(instring);
		// IncomingPost (public or private) or Owner (blank)
		getline(fin, instring);
		if (instring == "\tpublic") {
			isIncoming = true;
			isPublic = true;
		} else if (instring == "\tprivate") {
			isIncoming = true;
			isPublic = false;
		} else {
			isIncoming = isPublic = false;
		}
		// author or blank
		getline(fin, instring);
		if (isIncoming) {
			author = instring.erase(0,1);    // erase the \t
		}
		// push back this post to the appropriate user's messages_
		addIdPost(messageId, ownerId, message, likes, isIncoming, author, isPublic);
	}
	return count;
}

// Helper function for writePosts that compares two Post objects
bool compPosts(Post* p1, Post* p2) {
	return (p1->getMessageId() < p2->getMessageId());
}

int Network::writePosts(char* fname) {
	// prepare posts to write
	std::vector<Post*> all_posts;
	for (User* u : users_) {
		if (!u->getPosts().empty()) {    // if user has posts, add them
			for (Post * p : u->getPosts()) {
				all_posts.push_back(p);
			}
		}
	}
	// sort posts by messageId via helper function
	std::sort(all_posts.begin(), all_posts.end(), compPosts);

	// write file
	std::ofstream fout(fname);
	if (fout.bad()) return -1;
	fout << all_posts.size() << std::endl;
	for (Post *p : all_posts) {
		fout << p->getMessageId() << std::endl;
		fout << "\t" << p->getMessage() << std::endl;
		fout << "\t" << p->getOwnerId() << std::endl;
		fout << "\t" << p->getLikes() << std::endl;
		if (p->getAuthor()!="") {    // Incoming Post
			if (p->getIsPublic()) {
				fout << "\t" << "public" << std::endl;
			} else {
				fout << "\t" << "private" << std::endl;
			}
			fout << "\t" << p->getAuthor() << std::endl;
		} else {    // Owner Post
			fout << "\t" << "" << std::endl;
			fout << "\t" << "" << std::endl;
		}
	}
	fout.close();
	return all_posts.size();
}


int Network::readPolls(char* fname) {
	int count, messageId, ownerId, totalVotes, votes, val;
	std::string instring, message, option;
	std::set<int> voters;
	std::vector<PollOption*> options;
	// open file and get number of posts
	std::ifstream fin(fname);
	if (fin.bad()) return -1;
	getline(fin, instring);
	count = std::stoi(instring);

	while (getline(fin, instring)) {    // for each poll
		options.clear();    // clear previous poll's options
		messageId = std::stoi(instring);
		std::cout << "poll id " << messageId << std::endl;
		// ownerId
		getline(fin, instring);
		ownerId = std::stoi(instring);
		// message text
		getline(fin, message);
		message.erase(0,1);    // erase the \t
		// totalVotes
		getline(fin, instring);
		totalVotes = std::stoi(instring);
		while (getline(fin, instring) && instring != "") { // for each option
			// option name
			instring.erase(0,1);    // erase the \t
			option = instring;
			// votes
			getline(fin, instring);
			votes = std::stoi(instring);
			// voters
			voters.clear();    // clear any ids from prev option
			std::getline(fin, instring);
			std::stringstream ss(instring);
			while (ss >> val) {    // reads voter IDs until end
				voters.insert(val);
			}
			options.push_back(new PollOption(option, votes, voters));
		}
		// push back this post to the appropriate user's messages_
		addIdPoll(messageId, ownerId, message, options, totalVotes);
	}
	return count;
}

// Helper function for writePosts that compares two Poll objects
bool compPolls(Poll* p1, Poll* p2) {
	return (p1->getMessageId() < p2->getMessageId());
}

int Network::writePolls(char* fname) {
	// prepare polls to write
	std::vector<Poll*> all_polls;
	for (User* u : users_) {
		if (!u->getPolls().empty()) {    // if user has posts, add them
			for (Poll * p : u->getPolls()) {
				all_polls.push_back(p);
			}
		}
	}
	// sort posts by messageId via helper function
	std::sort(all_polls.begin(), all_polls.end(), compPolls);

	// write file
	std::ofstream fout(fname);
	if (fout.bad()) return -1;
	fout << all_polls.size() << std::endl;
	for (Poll *p : all_polls) {
		fout << p->getMessageId() << std::endl;
		fout << "\t" << p->getOwnerId() << std::endl;
		fout << "\t" << p->getMessage() << std::endl;
		fout << "\t" << p->getTotalVotes() << std::endl;
		for (auto opt : p->getOptions()) {
			fout << "\t" << opt->getOption() << std::endl;
			fout << "\t" << opt->getVotes() << std::endl;
			fout << "\t";
			for (int voter : opt->getVoters()) {
				fout << voter << " ";
			}
			fout << std::endl;
		}
		fout << "\n";
	}
	fout.close();
	return all_polls.size();
}
