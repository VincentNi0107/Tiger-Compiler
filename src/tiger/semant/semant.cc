#include "tiger/absyn/absyn.h"
#include "tiger/semant/semant.h"

namespace absyn {

void AbsynTree::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                           err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  root_->SemAnalyze(venv, tenv, 0, errormsg);
}

type::Ty *SimpleVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  env::EnvEntry *entry = venv->Look(sym_);
  if(entry && typeid(*entry) == typeid(env::VarEntry)){
    return (static_cast<env::VarEntry*>(entry))->ty_->ActualTy();
  }
  else{
    errormsg->Error(pos_, "undefined variable %s", sym_->Name().data());
  }
  return type::IntTy::Instance();
}

type::Ty *FieldVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *type = var_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(typeid(*type) == typeid(type::RecordTy)){
    auto field_it = (static_cast<type::RecordTy*>(type))->fields_->GetList().begin();
    auto field_end = (static_cast<type::RecordTy*>(type))->fields_->GetList().end();
    for(;field_it != field_end; ++field_it){
      if((*field_it)->name_ == sym_)
        return (*field_it)->ty_->ActualTy();
    }
    errormsg->Error(pos_,"field %s doesn't exist", sym_->Name().data());
  }
  else
    errormsg->Error(pos_,"not a record type");
  return type::VoidTy::Instance();
}

type::Ty *SubscriptVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   int labelcount,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *idx_type = subscript_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(typeid(*idx_type) == typeid(type::IntTy)){
    type::Ty *arr_type = var_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
    if(typeid(*arr_type) == typeid(type::ArrayTy)){
      return (static_cast<type::ArrayTy*>(arr_type))->ty_->ActualTy();
    }
    else{
      errormsg->Error(pos_,"array type required");
    }
  }
  else{
    errormsg->Error(pos_,"integer required");
  }
  return type::IntTy::Instance();
}

type::Ty *VarExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return var_->SemAnalyze(venv, tenv, labelcount, errormsg);
}

type::Ty *NilExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::NilTy::Instance();
}

type::Ty *IntExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::IntTy::Instance();
}

type::Ty *StringExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::StringTy::Instance();
}

type::Ty *CallExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  env::EnvEntry *func = venv->Look(func_);
  if(!func){
    errormsg->Error(pos_,"undefined function %s", func_->Name().data());
    return type::VoidTy::Instance();
  }
  if(typeid(*func) != typeid(env::FunEntry)){
    errormsg->Error(pos_,"%s not a function", func_->Name().data());
    return type::VoidTy::Instance();
  }
  type::TyList *formals = (static_cast<env::FunEntry*>(func))->formals_;
  if(formals->GetList().size() > args_->GetList().size()){
    errormsg->Error(pos_,"para type mismatch");
    return type::VoidTy::Instance();
  }
  if(formals->GetList().size() < args_->GetList().size()){
    errormsg->Error(pos_,"too many params in function %s", func_->Name().data());
    return type::VoidTy::Instance();
  }
  auto args_it = args_->GetList().begin();
  for(type::Ty* type : formals->GetList()){
    if(!(*args_it)->SemAnalyze(venv, tenv, labelcount, errormsg)->IsSameType(type)){
       errormsg->Error(pos_,"para type mismatch");
      return type::VoidTy::Instance();
    }
    args_it++;
  }
  return (static_cast<env::FunEntry*>(func))->result_;
}

type::Ty *OpExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *left_ty = left_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty *right_ty = right_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(oper_ == absyn::PLUS_OP || oper_ == absyn::MINUS_OP ||
    oper_ == absyn::TIMES_OP ||oper_ == absyn::DIVIDE_OP ||
    oper_ == absyn::AND_OP ||oper_ == absyn::OR_OP){
    if(typeid(*left_ty) != typeid(type::IntTy)){
      errormsg->Error(left_->pos_, "integer required");
    }
    if(typeid(*right_ty) != typeid(type::IntTy)){
      errormsg->Error(right_->pos_, "integer required");
    }
    return type::IntTy::Instance();
  }
  else{
    if(!left_ty->IsSameType(right_ty)){
      errormsg->Error(pos_, "same type required");
      return type::IntTy::Instance();
    }
  }
  return type::IntTy::Instance();
}

type::Ty *RecordExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *type = tenv->Look(typ_);
  if(!type){
    errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
    return type::VoidTy::Instance();
  }
  type = type->ActualTy();
  if(typeid(*type) != typeid(type::RecordTy)){
    errormsg->Error(pos_, "%s not a record type", typ_->Name().data());
    return type::VoidTy::Instance();
  }
  type::FieldList *type_fields = (static_cast<type::RecordTy*>(type))->fields_;
  if(fields_->GetList().size() != type_fields->GetList().size()){
    errormsg->Error(pos_, "%s wrong field size", typ_->Name().data());
    return type::VoidTy::Instance();
  }
  auto type_fields_it = type_fields->GetList().begin();
  for(EField * efield : fields_->GetList()){
    if(efield->name_ != (*type_fields_it)->name_){
      errormsg->Error(pos_, "field %s name error", efield->name_->Name().data());
      return type::VoidTy::Instance();
    }
    if(!efield->exp_->SemAnalyze(venv, tenv, labelcount, errormsg)->IsSameType((*type_fields_it)->ty_)){
      errormsg->Error(pos_, "field %s type error", efield->name_->Name().data());
      return type::VoidTy::Instance();
    };
    type_fields_it++;
  }
  return type;
}

type::Ty *SeqExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *type;
  for (Exp *exp : seq_->GetList()){
    type = exp->SemAnalyze(venv, tenv, labelcount, errormsg);
  }
  return type;
}

type::Ty *AssignExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  if(typeid(*var_) == typeid(absyn::SimpleVar)){
    env::EnvEntry *var_entry = venv->Look(static_cast<absyn::SimpleVar*>(var_)->sym_);
    if(!var_entry){
      errormsg->Error(pos_,"undefined variable %s", static_cast<absyn::SimpleVar*>(var_)->sym_->Name().data());
      return type::VoidTy::Instance();
    }
    if(var_entry->readonly_){
      errormsg->Error(pos_, "loop variable can't be assigned");
      return type::VoidTy::Instance();
    }
  }
  type::Ty *exp_type = exp_->SemAnalyze(venv, tenv, labelcount, errormsg);
  type::Ty *var_type = var_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if(!var_type->IsSameType(exp_type)){
    if(typeid(*var_type) != typeid(type::VoidTy))
      errormsg->Error(pos_, "unmatched assign exp");
  }
  return type::VoidTy::Instance();
}

type::Ty *IfExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *test_type = test_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty *then_type = then_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(typeid(*test_type)!= typeid(type::IntTy)){
    errormsg->Error(pos_, "test exp of if must be IntTy");
  }
  if(elsee_){
    type::Ty *elsee_type = elsee_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
    if(!then_type->IsSameType(elsee_type)){
      errormsg->Error(pos_, "then exp and else exp type mismatch");
      return type::VoidTy::Instance();
    }
    return then_type;
  }
  else if(typeid(*then_type) != typeid(type::VoidTy)){
    errormsg->Error(pos_, "if-then exp's body must produce no value");
  }
  return type::VoidTy::Instance();
}

type::Ty *WhileExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *test_type = test_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty *body_type = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg)->ActualTy();
  if(typeid(*test_type)!= typeid(type::IntTy)){
    errormsg->Error(pos_, "test exp of While needs to be IntTy");
  }
  if(typeid(*body_type) != typeid(type::VoidTy)){
    errormsg->Error(pos_, "while body must produce no value");
  }
  return type::VoidTy::Instance();
}

type::Ty *ForExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  venv->BeginScope();
  type::Ty *lo_type = lo_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty *hi_type = hi_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(typeid(*lo_type) != typeid(type::IntTy) || typeid(*hi_type) != typeid(type::IntTy)){
    errormsg->Error(pos_, "for exp's range type is not integer");
  }
  venv->Enter(var_, new env::VarEntry(type::IntTy::Instance(), true));  
  type::Ty *body_type = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg)->ActualTy();
  if(typeid(*body_type) != typeid(type::VoidTy)){
    errormsg->Error(pos_, "for's body must be a void exp");
  }
  venv->EndScope();
  return type::VoidTy::Instance();
}

type::Ty *BreakExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  if(labelcount == 0){
    errormsg->Error(pos_, "break is not inside any loop");
  }
  return type::VoidTy::Instance();

}

type::Ty *LetExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  venv->BeginScope();
  tenv->BeginScope();
  for(Dec *dec:decs_->GetList())
    dec->SemAnalyze(venv, tenv, labelcount, errormsg);

  type::Ty *result;
  if(!body_)
    result = type::VoidTy::Instance();
  else
    result = body_->SemAnalyze(venv, tenv, labelcount, errormsg);
  tenv->EndScope();
  venv->EndScope();
  return result;
}

type::Ty *ArrayExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *ty = tenv->Look(typ_);
  if(!ty){
    errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
  }
  ty = ty->ActualTy();
  type::Ty *size_type = size_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty *init_type = init_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(typeid(*size_type) != typeid(type::IntTy)){
    errormsg->Error(pos_, "int type required");
  }
  else if(typeid(*ty) != typeid(type::ArrayTy)){
    errormsg->Error(pos_,"array type required");
  }
  else if(!(static_cast<type::ArrayTy*>(ty))->ty_->IsSameType(init_type)){
    errormsg->Error(pos_, "type mismatch");
  }
  else{
    return ty;
  }
  return type::VoidTy::Instance();
}

type::Ty *VoidExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::VoidTy::Instance();
}

void FunctionDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto funcdec_it = functions_->GetList().begin();
  auto funcdec_end = functions_->GetList().end();
  for(;funcdec_it != funcdec_end; funcdec_it++){
    auto funcdec_it2 = funcdec_it;
    funcdec_it2++;
    for(;funcdec_it2 != funcdec_end;funcdec_it2++){
      if((*funcdec_it)->name_ == (*funcdec_it2)->name_){
        errormsg->Error(pos_, "two functions have the same name");
        return;
      }
    }
  }
  for(funcdec_it = functions_->GetList().begin();funcdec_it != funcdec_end; funcdec_it++){
    absyn::FieldList *params = (*funcdec_it)->params_;
    type::Ty *result_ty;
    if((*funcdec_it)->result_){
      result_ty = tenv->Look((*funcdec_it)->result_);
      if(!result_ty){
        errormsg->Error(pos_, "undefined type %s", (*funcdec_it)->result_->Name().data());
        result_ty = type::VoidTy::Instance();
      }
    }
    else
      result_ty = type::VoidTy::Instance();
    type::TyList *formals = params->MakeFormalTyList(tenv, errormsg);
    venv->Enter((*funcdec_it)->name_, new env::FunEntry(formals, result_ty));
  }
  for(funcdec_it = functions_->GetList().begin();funcdec_it != funcdec_end; funcdec_it++){
    env::FunEntry *fun_entry = static_cast<env::FunEntry*>(venv->Look((*funcdec_it)->name_));
    venv->BeginScope();
    absyn::FieldList *params = (*funcdec_it)->params_;
    type::TyList *formals = fun_entry->formals_;
    auto formal_it = formals->GetList().begin();
    auto param_it = params->GetList().begin();
    for(;param_it != params->GetList().end();formal_it++,param_it++)
      venv->Enter((*param_it)->name_, new env::VarEntry(*formal_it));
    type::Ty *ty = (*funcdec_it)->body_->SemAnalyze(venv, tenv, labelcount, errormsg);
    if(!ty->IsSameType(fun_entry->result_)){
      if(typeid(*fun_entry->result_) == typeid(type::VoidTy))
        errormsg->Error(pos_, "procedure returns value");
      else 
        errormsg->Error(pos_, "return type mismatch");    
    }
    venv->EndScope();
  }
}

void VarDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                        err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *init_ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if(typ_){
    type::Ty *expected_type = tenv->Look(typ_);
    if(!expected_type){
      errormsg->Error(pos_,"undefined type %s", typ_->Name().data());
    }
    else if(typeid(*init_ty) == typeid(type::NilTy)){
      expected_type = expected_type->ActualTy();
      if(typeid(*expected_type) != typeid(type::RecordTy)){
        errormsg->Error(pos_,"record type required");
      }
    }
    else if(!expected_type->IsSameType(init_ty)){
      errormsg->Error(pos_, "type mismatch");
    }
  }
  else if(typeid(*init_ty) == typeid(type::NilTy)){
    errormsg->Error(pos_,"init should not be nil without type specified");
  }
  venv->Enter(var_, new env::VarEntry(init_ty));
}

void TypeDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                         err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  std::list<sym::Symbol*> name_list;
  for(absyn::NameAndTy *type : types_->GetList()){
    for(sym::Symbol* type_name : name_list){
      if(type_name == type->name_){
        errormsg->Error(pos_, "two types have the same name");
        return;
      }
    }
    name_list.push_back(type->name_);
    tenv->Enter(type->name_, new type::NameTy(type->name_, NULL));
  }

  for(absyn::NameAndTy *type : types_->GetList()){
    type::Ty *name_type = tenv->Look(type->name_);
    type::Ty *tmp = type->ty_->SemAnalyze(tenv, errormsg);
    static_cast<type::NameTy*>(name_type)->ty_ = tmp;
    if(typeid(*tmp) == typeid(type::NameTy)){
      for(sym::Symbol* type_name : name_list){
        if(type_name == static_cast<type::NameTy*>(tmp)->sym_){
          errormsg->Error(pos_, "illegal type cycle");
          return;
        }
      }
    }
  }
}

type::Ty *NameTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *ty = tenv->Look(name_);
  if(!ty){
    errormsg->Error(pos_,"undefined type %s",name_->Name().data());
  }
  return new type::NameTy(name_, ty);
}

type::Ty *RecordTy::SemAnalyze(env::TEnvPtr tenv,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return new type::RecordTy(record_->MakeFieldList(tenv, errormsg));
}

type::Ty *ArrayTy::SemAnalyze(env::TEnvPtr tenv,
                              err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *ty = tenv->Look(array_);
  if(!ty){
    errormsg->Error(pos_,"undefined type %s",array_->Name().data());
  }
  return new type::ArrayTy(ty);
}

} // namespace absyn

namespace sem {

void ProgSem::SemAnalyze() {
  FillBaseVEnv();
  FillBaseTEnv();
  absyn_tree_->SemAnalyze(venv_.get(), tenv_.get(), errormsg_.get());
}
} // namespace sem
