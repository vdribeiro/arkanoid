#ifndef _MUSIC_H_
#define _MUSIC_H_

#include "timer.h"

/** @defgroup Music Music
 * @{
 *
 * Music related functions
 */

/** Song definition
 */
typedef struct {
  int lenght; /**< number of notes */
  int pause;  /**< pause between notes (ms) */
  int *notes; /**< array of notes */
  int *time;  /**< duration of each note (array) */
} Song;

/** Turns the speeker on
 */
void speaker_on();

/** Turns the speeker off
 */
void speaker_off();

/** Configura o timer com a frequencia da nota
*/
void programa_timer(int note);

/** Plays a song using busy-waiting
<pre>
  usage example:

  int n[] = {Sol6, Mi6, Sol6, Mi6};
  int t[] = {100, 100, 100, 100};
  Song s = { sizeof(n)/sizeof(n[0]), 10, n, t};
 
  play_song(&s);
</pre>
 */
void play_song(Song *s);

/** Natural notes names.
 * 's' means sharp, e.g. Ds is D#
 * The central octave is the fourth, thus C4 is the "central C"
 * 
 */
enum NotesN { C0, Cs0, D0, Ds0, E0, F0, Fs0, G0, Gs0, A0, As0, B0,
	      C1, Cs1, D1, Ds1, E1, F1, Fs1, G1, Gs1, A1, As1, B1,
	      C2, Cs2, D2, Ds2, E2, F2, Fs2, G2, Gs2, A2, As2, B2,
	      C3, Cs3, D3, Ds3, E3, F3, Fs3, G3, Gs3, A3, As3, B3,
	      C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4, A4, As4, B4,
	      C5, Cs5, D5, Ds5, E5, F5, Fs5, G5, Gs5, A5, As5, B5,
	      C6, Cs6, D6, Ds6, E6, F6, Fs6, G6, Gs6, A6, As6, B6,
	      C7, Cs7, D7, Ds7, E7, F7, Fs7, G7, Gs7, A7, As7, B7,
	      C8, Cs8, D8, Ds8};

/** French/Italian/Spanish/Portuguese notes names. 
 * 's' means sharp, e.g. Ds is D#
 * The central octave is the fourth, thus Do4 is the "central Do"
 */
enum NotesP { Do0, Dos0, Re0, Res0, Mi0, Fa0, Fas0, Sol0, Sols0, La0, Las0, Si0,
	      Do1, Dos1, Re1, Res1, Mi1, Fa1, Fas1, Sol1, Sols1, La1, Las1, Si1,
	      Do2, Dos2, Re2, Res2, Mi2, Fa2, Fas2, Sol2, Sols2, La2, Las2, Si2,
	      Do3, Dos3, Re3, Res3, Mi3, Fa3, Fas3, Sol3, Sols3, La3, Las3, Si3,
	      Do4, Dos4, Re4, Res4, Mi4, Fa4, Fas4, Sol4, Sols4, La4, Las4, Si4,
	      Do5, Dos5, Re5, Res5, Mi5, Fa5, Fas5, Sol5, Sols5, La5, Las5, Si5,
	      Do6, Dos6, Re6, Res6, Mi6, Fa6, Fas6, Sol6, Sols6, La6, Las6, Si6,
	      Do7, Dos7, Re7, Res7, Mi7, Fa7, Fas7, Sol7, Sols7, La7, Las7, Si7,
	      Do8, Dos8, Re8, Res8};

/** Notes frequency (Hz) (synchronized with NotesN and NotesP)
 *
 */
extern float frequency[];

/** @} end of music */

#endif
