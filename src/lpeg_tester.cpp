/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/// @brief Lua Parsing Expression Grammar Tester @file

#include "constants.h"
extern "C" {
#include "lua_sandbox.h"
}

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <lauxlib.h>
#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WPushButton>
#include <Wt/WServer>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WTree>
#include <Wt/WTreeNode>
#include <Wt/WVBoxLayout>

using namespace std;
namespace fs = boost::filesystem;

static const char* kMatchFunction = "_lpeg_match";

class Tester : public Wt::WApplication
{
public:
  Tester(const Wt::WEnvironment& env);

private:
  Wt::WWidget* Title();
  Wt::WWidget* Input();
  Wt::WWidget* Result();
  void TestGrammar(bool benchmark);
  void BenchmarkButton()
  {
    TestGrammar(true);
  }
  void GrammarButton()
  {
    TestGrammar(false);
  }
  void ShareGrammar();
  void LoadGrammar();
  bool CreateGlobalMatch(lua_State* lua);
  void Match(lua_sandbox* lsb, const string& input);
  void Benchmark(lua_sandbox* lsb, const string& input);

  void HandleInternalPath(const std::string& internalPath);
  void finalize();

  Wt::WTextArea* mInput;
  Wt::WTextArea* mGrammar;
  Wt::WContainerWidget* mResult;
};


Wt::WWidget* Tester::Title()
{
  Wt::WContainerWidget* container = new Wt::WContainerWidget();

  // Create a navigation bar with a link to a web page.
  Wt::WNavigationBar* navigation = new Wt::WNavigationBar(container);
  navigation->setTitle("LPeg Grammar Tester", "/");
  navigation->setResponsive(true);

  // Setup a Left-aligned menu.
  Wt::WMenu* leftMenu = new Wt::WMenu();
  navigation->addMenu(leftMenu);

  Wt::WPopupMenu* popup = new Wt::WPopupMenu();
  popup->setAutoHide(true, 250);
  popup->addItem("LPeg Documentation")->setLink(Wt::WLink("http://www.inf.puc-rio.br/~roberto/lpeg/lpeg.html"));
  Wt::WMenuItem* item = new Wt::WMenuItem("Documentation");
  item->setMenu(popup);
  leftMenu->addItem(item);

  popup = new Wt::WPopupMenu();
  popup->setAutoHide(true, 250);

  Wt::WMenuItem* pi = popup->addItem("date_time");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/date_time");
    HandleInternalPath(internalPath());
  }));

  pi = popup->addItem("common_log_format");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/clf");
    HandleInternalPath(internalPath());
  }));

  pi = popup->addItem("syslog");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/syslog");
    HandleInternalPath(internalPath());
  }));

  pi = popup->addItem("cbufd");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/cbufd");
    HandleInternalPath(internalPath());
  }));

  pi = popup->addItem("mysql");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/mysql");
    HandleInternalPath(internalPath());
  }));

  item = new Wt::WMenuItem("Modules");
  item->setMenu(popup);
  leftMenu->addItem(item);

  popup = new Wt::WPopupMenu();
  popup->setAutoHide(true, 250);

  pi = popup->addItem("Name-value lists");
  pi->setPathComponent("nvlist");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/nvlist");
    HandleInternalPath(internalPath());
  }));
  pi = popup->addItem("Split");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/split");
    HandleInternalPath(internalPath());
  }));
  pi = popup->addItem("CSV");
  pi->triggered().connect(std::bind([=]() {
    setInternalPath("/share/csv");
    HandleInternalPath(internalPath());
  }));
  item = new Wt::WMenuItem("Examples");
  item->setMenu(popup);
  leftMenu->addItem(item);

  return container;
}


Wt::WWidget* Tester::Input()
{

  Wt::WContainerWidget* container = new Wt::WContainerWidget();
  container->setStyleClass("input_container");

  Wt::WText* t = new Wt::WText("Input", container);
  t->setStyleClass("area_title");
  new Wt::WBreak(container);
  mInput = new Wt::WTextArea(container);
  mInput->setText("20131220T094700");

  new Wt::WBreak(container);

  t = new Wt::WText("Grammar", container);
  t->setStyleClass("area_title");
  new Wt::WBreak(container);
  mGrammar = new Wt::WTextArea(container);
  mGrammar->setRows(15);
  mGrammar->setText("local l = require 'lpeg'\nl.locale(l)\ngrammar = l.C(l.digit^-4)");

  Wt::WPushButton* button = new Wt::WPushButton("Test Grammar", container);
  button->clicked().connect(this, &Tester::GrammarButton);

  button = new Wt::WPushButton("Benchmark Grammar", container);
  button->clicked().connect(this, &Tester::BenchmarkButton);

  button = new Wt::WPushButton("Share Grammar", container);
  button->clicked().connect(this, &Tester::ShareGrammar);

  return container;
}


Wt::WWidget* Tester::Result()
{
  Wt::WContainerWidget* c = new Wt::WContainerWidget();
  c->setStyleClass("result_container");

  Wt::WText* t = new Wt::WText("Results", c);
  t->setStyleClass("area_title");

  mResult = new Wt::WContainerWidget(c);
  mResult->setStyleClass("result_output");

  return c;
}


void OutputItem(lua_State* lua, int i, Wt::WTreeNode* parent, stringstream& ss);


void OutputTable(lua_State* lua, int i, Wt::WTreeNode* parent)
{
  int result = 0;
  lua_checkstack(lua, 2);
  lua_pushnil(lua);
  if (i < 0) {
    --i;
  }
  while (result == 0 && lua_next(lua, i) != 0) {
    stringstream ss;
    switch (lua_type(lua, -2)) {
    case LUA_TNUMBER:
      ss << "[" << lua_tonumber(lua, -2) << "] = ";
      break;
    case LUA_TSTRING:
      ss << lua_tostring(lua, -2) << " = ";
      break;
    }
    OutputItem(lua, -1, parent, ss);
    lua_pop(lua, 1); // Remove the value leaving the key on top for
                     // the next interation.
  }
}


void OutputItem(lua_State* lua, int i, Wt::WTreeNode* parent, stringstream& ss)
{
  switch (lua_type(lua, i)) {
  case LUA_TNUMBER:
    ss <<  lua_tonumber(lua, i);
    parent->addChildNode(new Wt::WTreeNode(ss.str()));
    break;
  case LUA_TSTRING:
    ss << "\"" << lua_tostring(lua, i) << "\"";
    parent->addChildNode(new Wt::WTreeNode(ss.str()));
    break;
  case LUA_TBOOLEAN:
    if (lua_toboolean(lua, i)) {
      ss << "true";
      parent->addChildNode(new Wt::WTreeNode(ss.str()));
    } else {
      ss << "false";
      parent->addChildNode(new Wt::WTreeNode(ss.str()));
    }
    break;
  case LUA_TTABLE:
    {
      ss << "table";
      Wt::WTreeNode* n = new Wt::WTreeNode(ss.str());
      n->setLoadPolicy(Wt::WTreeNode::NextLevelLoading);
      parent->addChildNode(n);
      OutputTable(lua, i, n);
      n->expand();
      break;
    }
  default:
    parent->addChildNode(new Wt::WTreeNode(luaL_typename(lua, i)));
    break;
  }

}


bool Tester::CreateGlobalMatch(lua_State* lua)
{
  lua_getglobal(lua, "lpeg");
  if (!lua_istable(lua, -1)) {
    lua_pop(lua, 1); // remove the global
    return false;
  }

  lua_getfield(lua, -1, "match");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 2); // remove lpeg and field
    return false;
  }

  lua_setglobal(lua, kMatchFunction);
  lua_pop(lua, 1); // remove the lpeg table

  return true;
}


void Tester::Match(lua_sandbox* lsb, const string& input)
{
  lua_State* lua = lsb_get_lua(lsb);
  if (!lua) return;

  if (!CreateGlobalMatch(lua)) {
    stringstream ss;
    ss << "lpeg.match is not available";
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    return;
  }

  if (lsb_pcall_setup(lsb, kMatchFunction)) {
    stringstream ss;
    ss << "lsb_pcall_setup() failed";
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    return;
  }

  lua_getglobal(lua, "grammar");
  if (!lua_isuserdata(lua, -1)) {
    stringstream ss;
    ss << "no global grammar variable was found";
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    return;
  }

  lua_pushstring(lua, input.c_str());
  if (lua_pcall(lua, 2, LUA_MULTRET, 0) != 0) {
    char err[LSB_ERROR_SIZE];
    int len = snprintf(err, LSB_ERROR_SIZE, "%s() %s", kMatchFunction,
                       lua_tostring(lua, -1));
    if (len >= LSB_ERROR_SIZE || len < 0) {
      err[LSB_ERROR_SIZE - 1] = 0;
    }
    stringstream ss;
    ss << err;
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    lsb_terminate(lsb, err);
    return;
  }
  // iterater over the results
  int results = lua_gettop(lua);
  if (LUA_TNIL == lua_type(lua, 1)) {
    stringstream ss;
    ss << "no match";
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
  } else {
    Wt::WTree* tree = new Wt::WTree(mResult);
    tree->setSelectionMode(Wt::SingleSelection);
    Wt::WTreeNode* root = new Wt::WTreeNode("Returned Values");
    root->setStyleClass("tree_results");
    tree->setTreeRoot(root);
    root->label()->setTextFormat(Wt::PlainText);
    root->setLoadPolicy(Wt::WTreeNode::NextLevelLoading);
    for (int i = 1; i <= results; ++i) {
      stringstream ss;
      OutputItem(lua, i, root, ss);
    }
    root->expand();
  }
  lua_pop(lua, lua_gettop(lua)); // clear the stack
  lsb_pcall_teardown(lsb);
}


void Tester::Benchmark(lua_sandbox* lsb, const string& input)
{
  lua_State* lua = lsb_get_lua(lsb);
  if (!lua) return;
  bool nomatch = false;

  if (!CreateGlobalMatch(lua)) {
    stringstream ss;
    ss << "lpeg.match is not available";
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    return;
  }

  clock_t t = clock();
  int x, iter = 10000;
  for (x = 0; x < iter; ++x) {
    if (lsb_pcall_setup(lsb, kMatchFunction)) {
      stringstream ss;
      ss << "lsb_pcall_setup() failed";
      Wt::WText* t = new Wt::WText(ss.str(), mResult);
      t->setStyleClass("result_error");
      Wt::log("info") << ss.str();
      return;
    }

    lua_getglobal(lua, "grammar");
    if (!lua_isuserdata(lua, -1)) {
      stringstream ss;
      ss << "no global grammar variable was found";
      Wt::WText* t = new Wt::WText(ss.str(), mResult);
      t->setStyleClass("result_error");
      Wt::log("info") << ss.str();
      return;
    }

    lua_pushstring(lua, input.c_str());
    if (lua_pcall(lua, 2, LUA_MULTRET, 0) != 0) {
      char err[LSB_ERROR_SIZE];
      int len = snprintf(err, LSB_ERROR_SIZE, "%s() %s", kMatchFunction,
                         lua_tostring(lua, -1));
      if (len >= LSB_ERROR_SIZE || len < 0) {
        err[LSB_ERROR_SIZE - 1] = 0;
      }
      stringstream ss;
      ss << err;
      Wt::WText* t = new Wt::WText(ss.str(), mResult);
      t->setStyleClass("result_error");
      Wt::log("info") << ss.str();
      lsb_terminate(lsb, err);
      return;
    } else {
      if (LUA_TNIL == lua_type(lua, 1)) {
        nomatch = true;
      }
    }
    lua_pop(lua, lua_gettop(lua)); // clear the stack
  }
  t = clock() - t;
  stringstream ss;
  Wt::WText* txt = new Wt::WText(mResult);
  if (nomatch) {
    txt->setStyleClass("result_error");
    ss << " *** MATCH FAILED ***<br/>";
  }
  ss << "Benchmark"
    << "<br/>samples: " << x
    << "<br/>seconds per match: " << (((float)t) / CLOCKS_PER_SEC / x)
    << "<br/>max memory (bytes): " << lsb_usage(lsb, LSB_UT_MEMORY, LSB_US_MAXIMUM)
    << "<br/>max Lua instructions: " << lsb_usage(lsb, LSB_UT_INSTRUCTION, LSB_US_MAXIMUM);
  txt->setText(ss.str());
  lsb_pcall_teardown(lsb);
}


void Tester::TestGrammar(bool benchmark)
{
  mResult->clear();

  fs::path grammar = fs::path("/tmp") / (sessionId() + ".lua");
  ofstream ofs(grammar.string().c_str());
  if (ofs) {
    ofs << mGrammar->text();
    ofs.close();
  } else {
    stringstream ss;
    ss << "failed to open: " << grammar;
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("error") << ss.str();
    return;
  }
  lua_sandbox* sb = lsb_create(NULL, grammar.string().c_str(), "modules",
                               8*1024*1024, 1e6, 1024*63);
  if (!sb) {
    stringstream ss;
    ss << "lsb_create() failed";
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("error") << ss.str();
    return;
  }
  if (lsb_init(sb, nullptr)) {
    stringstream ss;
    string error = lsb_get_error(sb);
    size_t pos = error.find_first_of(':');
    if (pos != string::npos) {
      ss << "line " << error.substr(pos + 1);
    } else {
      ss << error;
    }
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    return;

  }
  if (benchmark) {
    Benchmark(sb, mInput->text().narrow());
  } else {
    Match(sb, mInput->text().narrow());
  }
  char* e = lsb_destroy(sb, nullptr);
  if (e) {
    stringstream ss;
    ss << "lsb_destroy() failed: " << e;
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    free(e);
  }
}


void Tester::ShareGrammar()
{
  boost::hash<std::string> string_hash;
  string data(mInput->text().narrow() + mGrammar->text().narrow());
  string h = boost::lexical_cast<string>(string_hash(data));
  fs::path input = fs::path(appRoot()) / "share" / (h + ".data");
  if (!exists(input)) {
    ofstream data_fs(input.string().c_str());
    if (data_fs) {
      data_fs << mInput->text();
      data_fs.close();
    } else {
      stringstream ss;
      ss << "failed to open: " << input;
      Wt::WText* t = new Wt::WText(ss.str(), mResult);
      t->setStyleClass("result_error");
      Wt::log("error") << ss.str();
      return;
    }

    fs::path grammar = fs::path(appRoot()) / "share" / (h + ".lua");
    ofstream lua_fs(grammar.string().c_str());
    if (lua_fs) {
      lua_fs << mGrammar->text();
      lua_fs.close();
    } else {
      stringstream ss;
      ss << "failed to open: " << grammar;
      Wt::WText* t = new Wt::WText(ss.str(), mResult);
      t->setStyleClass("result_error");
      Wt::log("error") << ss.str();
      return;
    }
  }

  setInternalPath("/share/" + h);
  Wt::WMessageBox* messageBox = new Wt::WMessageBox("Share", makeAbsoluteUrl(bookmarkUrl()), Wt::Information, Wt::Ok);
  messageBox->buttonClicked().connect(std::bind([=]() {
    delete messageBox;
  }));
  messageBox->show();
}


void Tester::LoadGrammar()
{
  string h = internalPathNextPart("/share/");
  fs::path input = fs::path(appRoot()) / "share" / (h + ".data");
  mInput->setText("");
  mGrammar->setText("");
  mResult->clear();

  ifstream data_fs(input.string().c_str());
  if (data_fs) {
    data_fs.unsetf(std::ios::skipws);
    string data((istream_iterator<char>(data_fs)), istream_iterator<char>());
    mInput->setText(data);
    data_fs.close();
  } else {
    stringstream ss;
    ss << "share not found: " << h;
    Wt::WText* t = new Wt::WText(ss.str(), mResult);
    t->setStyleClass("result_error");
    Wt::log("info") << ss.str();
    return;
  }

  fs::path grammar = fs::path(appRoot()) / "share" / (h + ".lua");
  ifstream grammar_fs(grammar.string().c_str());
  if (grammar_fs) {
    grammar_fs.unsetf(std::ios::skipws);
    string data((istream_iterator<char>(grammar_fs)), istream_iterator<char>());
    mGrammar->setText(data);
    grammar_fs.close();
  }
}


void Tester::HandleInternalPath(const std::string& internalPath)
{
  if (internalPathMatches("/share")) {
    LoadGrammar();
  } else {
    if (internalPath != "/") {
      setInternalPath("/",  true);
    }
  }
}


Tester::Tester(const Wt::WEnvironment& env)
  : Wt::WApplication(env)
{

  setTheme(new Wt::WBootstrapTheme());
  useStyleSheet("resources/lpeg_tester.css");

  Wt::WContainerWidget* container = new Wt::WContainerWidget();
  container->setStyleClass("page");

  Wt::WVBoxLayout* vbox = new Wt::WVBoxLayout();
  container->setLayout(vbox);

  vbox->addWidget(Title(), 1);

  Wt::WHBoxLayout* hbox = new Wt::WHBoxLayout();
  vbox->addLayout(hbox);

  hbox->addWidget(Input(), 1);
  hbox->addWidget(Result(), 1);

  root()->addWidget(container);

  HandleInternalPath(internalPath());
  internalPathChanged().connect(this, &Tester::HandleInternalPath);
}


void Tester::finalize()
{
  fs::path grammar = fs::path("/tmp") / (sessionId() + ".lua");
  if (exists(grammar)) {
    remove(grammar);
  }
}


Wt::WApplication* CreateApplication(const Wt::WEnvironment& aEnv)
{
  return new Tester(aEnv);
}


int main(int argc, char** argv)
{
  try {
    Wt::WServer server(argv[0], trink::lpeg_tester::kWtConfig);
    server.setServerConfiguration(argc, argv);

    server.addEntryPoint(Wt::Application, CreateApplication);
    if (server.start()) {
      string uid;
      if (server.readConfigurationProperty("uid", uid)) {
        if (setuid(boost::lexical_cast<int>(uid))) {
          Wt::log("fatal") << "Failed to setuid";
          return EXIT_FAILURE;
        }
      }
      int sig = Wt::WServer::waitForShutdown(argv[0]);
      Wt::log("info") << "Shutdown (signal = " << sig << ")";
      server.stop();
    } else {
      Wt::log("fatal") << "Failed to start the user interface";
      return EXIT_FAILURE;
    }
  }
  catch (const exception& e) {
    Wt::log("fatal") << "std exception: " << e.what();
    return EXIT_FAILURE;
  }
  catch (...) {
    Wt::log("fatal") << "unknown exception";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
