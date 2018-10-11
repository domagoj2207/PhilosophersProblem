# PhilosophersProblem

Solution for Dining philosophers problem in C programming language using pipelines and Ricart&Agrawal protocol for synchronization.

Task is a lab assignment in a graduate subject "Advanced operating systems" on the Faculty of Electrical Engineering and Computing, Zagreb Croatia.
Full text of an assignment in Croatian:


  "Na nekoj konferenciji okupilo se N filozofa. Za razliku od poznatog slučaja 5 filozofa, ovdje za stolom ima mjesta samo za jednu osobu, a ne za 5. Dakle, pristup stolu je kritični odsječak jer samo jedan filozof može biti u nekom trenutku za stolom ili je stol prazan. Na početku glavni proces stvara N procesa filozofa (broj N>2 se zadaje). Procesi međusobno komuniciraju cjevovodima (svejedno: običnih ili imenovanih).

proces filozof{
   sudjeluj na konferenciji; // sleep(1);
   pristupi stolu, jedi i ispiši "Filozof i je za stolom";    // sleep(3); (kritični odsječak)
   sudjeluj na konferenciji; // sleep(1);
}

Sinkronizirati N procesa filozofa koristeći protokol Ricarta i Agrawala.

Svi procesi ispisuju poruku koju šalju i poruku koju primaju."
