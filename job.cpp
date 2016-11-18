#include "shell.h"

using namespace std;

job::job(int PID, int JID, string command) {
  this->PID = PID;
  this->JID = JID;
  this->command = command;
  this->status = "Running";
}

job::~job() {

}

int job::getPID() {
  return this->PID;
}

int job::getJID() {
  return this->JID;
}

void job::setJID(int JID) {
  this->JID = JID;
}

void job::setStatus(string status) {
  this->status = status;
}

string job::getCmd() {
  return this->command;
}

string job::getStatus() {
  return this->status;
}
