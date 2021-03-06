/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  // TODO
  Type* dup = (Type*) malloc(sizeof(Type));
  dup->typeClass = type->typeClass;
  if(dup->typeClass == TP_ARRAY) {
    dup->arraySize = type->arraySize;
    dup->elementType = type->elementType;
  }
  return dup;
}

int compareType(Type* type1, Type* type2) {
  // TODO
  if(type1->typeClass == type2->typeClass) {
    if(type1->typeClass == TP_ARRAY) {
      if(type1->arraySize == type2->arraySize) {
        if(compareType(type1->elementType,type2->elementType))
          return 1;
      }
      else return 0;
    }
    else return 1;
  }
  return 0;
}

void freeType(Type* type) {
  // TODO
  if (type->elementType != NULL)
    freeType(type->elementType);
  free(type);
  type = NULL;
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  // TODO
  ConstantValue* constVal = (ConstantValue*) malloc(sizeof(ConstantValue));
  constVal->type = TP_INT;
  constVal->intValue = i;
  return constVal;
}

ConstantValue* makeCharConstant(char ch) {
  // TODO
  ConstantValue* constVal = (ConstantValue*) malloc(sizeof(ConstantValue));
  constVal->type = TP_CHAR;
  constVal->charValue = ch;
  return constVal;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // TODO
  ConstantValue* dup = (ConstantValue*) malloc(sizeof(ConstantValue));
  dup->type = v->type;
  if(v->type == TP_INT)
    dup->intValue = v->intValue;
  else
    dup->charValue = v->charValue;
  return dup;
}

/******************* Object utilities ******************************/

Scope* CreateScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* CreateProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = CreateScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* CreateConstantObject(char *name) {
  // TODO
  Object* constObj = (Object*) malloc(sizeof(Object));
  strcpy(constObj->name, name);
  constObj->kind = OBJ_CONSTANT;
  constObj->constAttrs = (ConstantAttributes*) malloc(sizeof(ConstantAttributes));
  constObj->constAttrs->value = NULL;
  return constObj;
}

Object* CreateTypeObject(char *name) {
  // TODO
  Object* typeObj = (Object*) malloc(sizeof(Object));
  strcpy(typeObj->name, name);
  typeObj->kind = OBJ_TYPE;
  typeObj->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  typeObj->typeAttrs->actualType = NULL;
  return typeObj;
}

Object* CreateVariableObject(char *name) {
  // TODO
  Object* varObj = (Object*) malloc(sizeof(Object));
  strcpy(varObj->name, name);
  varObj->kind = OBJ_VARIABLE;
  varObj->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  varObj->varAttrs->type = NULL;
  varObj->varAttrs->scope = symtab->currentScope;
  return varObj;
}

Object* CreateFunctionObject(char *name) {
  // TODO
  Object* funcObj = (Object*) malloc(sizeof(Object));
  strcpy(funcObj->name, name);
  funcObj->kind = OBJ_FUNCTION;
  funcObj->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  funcObj->funcAttrs->returnType = NULL;
  funcObj->funcAttrs->paramList = NULL;
  funcObj->funcAttrs->scope = CreateScope(funcObj, symtab->currentScope);
  return funcObj;
}

Object* CreateProcedureObject(char *name) {
  // TODO
  Object* proObj = (Object*) malloc(sizeof(Object));
  strcpy(proObj->name, name);
  proObj->kind = OBJ_PROCEDURE;
  proObj->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  proObj->procAttrs->paramList = NULL;
  proObj->procAttrs->scope = CreateScope(proObj, symtab->currentScope);
  return proObj;
}

Object* CreateParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // TODO
  Object* paramObj = (Object*) malloc(sizeof(Object));
  strcpy(paramObj->name, name);
  paramObj->kind = OBJ_PARAMETER;
  paramObj->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  paramObj->paramAttrs->kind = kind;
  paramObj->paramAttrs->type = NULL;
  paramObj->paramAttrs->function = owner;
  return paramObj;
}

void freeObject(Object* obj) {
  // TODO
  if(obj != NULL) {
    if (obj->kind == OBJ_CONSTANT) {
      if(obj->constAttrs->value != NULL) {
        free(obj->constAttrs->value);
        obj->constAttrs->value = NULL;
      }
    }
    else if(obj->kind == OBJ_VARIABLE) {
      if(obj->varAttrs->type != NULL) {
        free(obj->varAttrs->type);
        obj->varAttrs->type = NULL;
      }
    }
    else if(obj->kind == OBJ_TYPE) {
      if (obj->typeAttrs->actualType != NULL) {
        free(obj->typeAttrs->actualType);
        obj->typeAttrs->actualType = NULL;
      }
    }
    else if(obj->kind == OBJ_FUNCTION) {
      freeScope(obj->funcAttrs->scope);
    }
    else if(obj->kind == OBJ_PROCEDURE) {
      freeScope(obj->funcAttrs->scope);
    }
    else if(obj->kind == OBJ_PARAMETER) {
      if (obj->paramAttrs->type != NULL) {
        free(obj->paramAttrs->type);
        obj->paramAttrs->type = NULL;
      }
    }
    else if(obj->kind == OBJ_PROGRAM) {
      if (obj->progAttrs->scope != NULL) {
        free(obj->progAttrs->scope);
        obj->progAttrs->scope = NULL;
      }
    }
    free(obj);
    obj = NULL;
  }
}

void freeScope(Scope* scope) {
  // TODO
  if (scope != NULL) {
    freeObjectList(scope->objList);
    free(scope);
    scope = NULL;
  }
}

void freeObjectList(ObjectNode *objList) {
  // TODO
  if (objList != NULL) {
    freeObject(objList->object);
    freeObjectList(objList->next);
    objList = NULL;
  }
}

void freeReferenceList(ObjectNode *objList) {
  // TODO
  if (objList != NULL) {
    freeReferenceList(objList->next);
    objList = NULL;
  }
}

void AddObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // TODO
  while(objList != NULL){
    if (strcmp(objList->object->name, name) == 0) {
      return objList->object;
    }
    objList = objList->next;
  }
  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = CreateFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEI");
  param = CreateParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEC");
  param = CreateParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITELN");
  AddObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      AddObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      AddObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  AddObject(&(symtab->currentScope->objList), obj);
}


