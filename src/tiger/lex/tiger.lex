%filenames = "scanner"

 /*
  * Please don't modify the lines above.
  */

 /* You can add lex definitions here. */
 /* TODO: Put your lab2 code here */

%x COMMENT STR IGNORE ESCAPE CONTROL

%%

 /*
  * Below is examples, which you can wipe out
  * and write regular expressions and actions of your own.
  *
  * All the tokens:
  *   Parser::ID
  *   Parser::STRING
  *   Parser::INT
  *   Parser::COMMA
  *   Parser::COLON
  *   Parser::SEMICOLON
  *   Parser::LPAREN
  *   Parser::RPAREN
  *   Parser::LBRACK
  *   Parser::RBRACK
  *   Parser::LBRACE
  *   Parser::RBRACE
  *   Parser::DOT
  *   Parser::PLUS
  *   Parser::MINUS
  *   Parser::TIMES
  *   Parser::DIVIDE
  *   Parser::EQ
  *   Parser::NEQ
  *   Parser::LT
  *   Parser::LE
  *   Parser::GT
  *   Parser::GE
  *   Parser::AND
  *   Parser::OR
  *   Parser::ASSIGN
  *   Parser::ARRAY
  *   Parser::IF
  *   Parser::THEN
  *   Parser::ELSE
  *   Parser::WHILE
  *   Parser::FOR
  *   Parser::TO
  *   Parser::DO
  *   Parser::LET
  *   Parser::IN
  *   Parser::END
  *   Parser::OF
  *   Parser::BREAK
  *   Parser::NIL
  *   Parser::FUNCTION
  *   Parser::VAR
  *   Parser::TYPE
  */

 /* reserved words */
 /* TODO: Put your lab2 code here */
"let" {adjust(); return Parser::LET;}
"type" {adjust(); return Parser::TYPE;}
"=" {adjust(); return Parser::EQ;}
"array" {adjust(); return Parser::ARRAY;}
"of" {adjust(); return Parser::OF;}
"var" {adjust(); return Parser::VAR;}
":" {adjust(); return Parser::COLON;}
":=" {adjust(); return Parser::ASSIGN;}
"[" {adjust(); return Parser::LBRACK;}
"]" {adjust(); return Parser::RBRACK;}
"in" {adjust(); return Parser::IN;}
"end" {adjust(); return Parser::END;}
"{" {adjust(); return Parser::LBRACE;}
"}" {adjust(); return Parser::RBRACE;}
"," {adjust(); return Parser::COMMA;}
"." {adjust(); return Parser::DOT;}
";" {adjust(); return Parser::SEMICOLON;}
"function" {adjust(); return Parser::FUNCTION;}
"(" {adjust(); return Parser::LPAREN;}
")" {adjust(); return Parser::RPAREN;}
"*" {adjust(); return Parser::TIMES;}
"-" {adjust(); return Parser::MINUS;}
"nil" {adjust(); return Parser::NIL;}
"+" {adjust(); return Parser::PLUS;}
">" {adjust(); return Parser::GT;}
"while" {adjust(); return Parser::WHILE;}
"do" {adjust(); return Parser::DO;}
"else" {adjust(); return Parser::ELSE;}
"if" {adjust(); return Parser::IF;}
"/" {adjust(); return Parser::DIVIDE;}
"<>" {adjust(); return Parser::NEQ;}
"<" {adjust(); return Parser::LT;}
"<=" {adjust(); return Parser::LE;}
">=" {adjust(); return Parser::GE;}
"&" {adjust(); return Parser::AND;}
"|" {adjust(); return Parser::OR;}
"to" {adjust(); return Parser::TO;}
"for" {adjust(); return Parser::FOR;}
"break" {adjust(); return Parser::BREAK;}
"then" {adjust(); return Parser::THEN;}
[[:digit:]]+ {adjust(); return Parser::INT;}
[[:alpha:]]([[:alnum:]]|_)* {adjust();return Parser::ID;}
"/*" {adjust(); comment_level_ = 1; begin(StartCondition__::COMMENT);}
<COMMENT>{
  "/*" {adjust(); comment_level_ += 1;}
  "*/" {
    adjust();
    comment_level_ -= 1;
    if(comment_level_ == 0){
      begin(StartCondition__::INITIAL);
      }
    }
  .|\n {adjust();}
}
\" {adjust(); begin(StartCondition__::STR);}
<STR>{
  \" {
    adjustStr();
    setMatched(string_buf_);
    string_buf_.clear();
    begin(StartCondition__::INITIAL);
    return Parser::STRING;
  }
  \\ {
    adjustStr();
    begin(StartCondition__::ESCAPE);
  }
  .|\n {
    adjustStr();
    string_buf_ += matched();
  }
}
<ESCAPE>{
  n {
    adjustStr();
    string_buf_ +='\n';
    begin(StartCondition__::STR);
  }
  t {
    adjustStr();
    string_buf_ +='\t';
    begin(StartCondition__::STR);
  }
  \\ {
    adjustStr();
    string_buf_ +='\\';
    begin(StartCondition__::STR);
  }
  \" {
    adjustStr();
    string_buf_ +='\"';
    begin(StartCondition__::STR);
  }
  [0-9]{3} {
    adjustStr();
    string_buf_ +=(char)atoi(matched().data());
    begin(StartCondition__::STR);    
  }
  "^" {
    adjustStr();
    begin(StartCondition__::CONTROL);
  }
  [[:blank:]]{+}[\n] {
    adjustStr();
    begin(StartCondition__::IGNORE);
  }
}
<CONTROL> {
  A {
    string_buf_ += (char)1;
    adjustStr();
    begin(StartCondition__::STR);
  }
  C {
    string_buf_ += (char)3;
    adjustStr();
    begin(StartCondition__::STR);
  }
  O {
    string_buf_ += (char)15;
    adjustStr();
    begin(StartCondition__::STR);
  }
  M {
    string_buf_ += (char)13;
    adjustStr();
    begin(StartCondition__::STR);
  }
  P {
    string_buf_ += (char)16;
    adjustStr();
    begin(StartCondition__::STR);
  }
  I {
    string_buf_ += (char)9;
    adjustStr();
    begin(StartCondition__::STR);
  }
  L {
    string_buf_ += (char)12;
    adjustStr();
    begin(StartCondition__::STR);
  }
  E {
    string_buf_ += (char)5;
    adjustStr();
    begin(StartCondition__::STR);
  }
  R {
    string_buf_ += (char)18;
    adjustStr();
    begin(StartCondition__::STR);
  }
  S {
    string_buf_ += (char)19;
    adjustStr();
    begin(StartCondition__::STR);
  }
}
<IGNORE> {
  ([[:blank:]]{+}[\n])* {
    adjustStr();
  }
  \\ {
    adjustStr();
    begin(StartCondition__::STR);
  }
}

 /*
  * skip white space chars.
  * space, tabs and LF
  */
[ \t]+ {adjust();}
\n {adjust(); errormsg_->Newline();}

 /* illegal input */
. {adjust(); errormsg_->Error(errormsg_->tok_pos_, "illegal token");}
