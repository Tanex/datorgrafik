person(kalle).
person(erik).
person(nisse).
father(kalle,erik).
father(nisse,kalle).

ancestor(X,Y) :- father(X,Y).
ancestor(X,Y) :- father(X,Z), ancestor(Z,Y).

