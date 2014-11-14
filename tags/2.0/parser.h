#ifndef BISON_PARSER_H
# define BISON_PARSER_H

#ifndef YYSTYPE
typedef union {
  size_t synch;
  StateFormula* formula;
  const PathFormula* path;
  const Distribution* dist;
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
# define	EXP	264
# define	MODULE	265
# define	ENDMODULE	266
# define	PNAME	267
# define	NAME	268
# define	NUMBER	269
# define	ARROW	270
# define	DOTDOT	271
# define	ILLEGAL_TOKEN	272
# define	IMPLY	273
# define	LTE	274
# define	GTE	275
# define	EQ	276
# define	NEQ	277


extern YYSTYPE yylval;

#endif /* not BISON_PARSER_H */
