#ifndef BISON_PARSER_H
# define BISON_PARSER_H

#ifndef YYSTYPE
typedef union {
  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Expression* expr;
  Range range;
  int nat;
  const std::string* str;
  const Rational* num;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	STOCHASTIC	257
# define	CONST	258
# define	RATE	259
# define	GLOBAL	260
# define	INIT	261
# define	TRUE_TOKEN	262
# define	FALSE_TOKEN	263
# define	MODULE	264
# define	ENDMODULE	265
# define	PNAME	266
# define	NAME	267
# define	NUMBER	268
# define	ARROW	269
# define	DOTDOT	270
# define	ILLEGAL_TOKEN	271
# define	IMPLY	272
# define	LTE	273
# define	GTE	274
# define	EQ	275
# define	NEQ	276


extern YYSTYPE yylval;

#endif /* not BISON_PARSER_H */
