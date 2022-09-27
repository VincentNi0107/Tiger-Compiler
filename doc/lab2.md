# Document of lab2
## Handling comments
* Enter COMMENT condition when meeting /*, set comment_level_ = 1.
* Increase comment_level_ when meeting /* in the COMMENT condition.
* Derease comment_level_ when meeting */ in the COMMENT condition. If comment_level_ == 0, return to the INITIAL condition.
## Handling strings
* Enter STR condition when meeting ".
* Return to INITIAL condition when meeting " in the STR condition.
* Use adjustStr() instead of adjust() inside STR condition to avoid updating token_pos during the processing of string.
* Use string_buf_ to store the string.
* Enter ESCAPE condition to process escape character.
* Enter IGNORE condition when meeting blank in the ESCAPE condition.
