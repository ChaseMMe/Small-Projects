"Pong"

Chase Meadows
8/18/2022


DESCRIPTION:
First game I've ever coded.

Basic Pong game with 1- and 2-player options using raylib framework.
https://www.raylib.com/index.html

Game was started using this YouTube tutorial:
https://youtu.be/LvpS3ILwQNA
until about 32:00 when I mostly took over, coming back once or twice for help with things like displaying text.
I had decided on using raylib to jump in, but I had no idea how to even begin using a framework since all my experience prior had been console programs.
I'd also never really used Visual Studio and figured I should start learning an IDE, so this seemed like a good place to start.



PURPOSE:
My only real goal was to figure out the first steps of turning written code into a playable game. 
I messed around with engines a bit and will again, but even there I mostly just wanted to start typing, so I shifted to this.
Figuring out how to start any new skill is always the hardest, so I opted to let a tutorial walk me through the initial steps. 
I was confident in my ability to take over as soon as I understood the general idea of how a framework was used, so I wanted to make sure I
added a few things of my own to the project before shelving it.



RESULT:
I feel ready to move on to a new project without any direct walkthrough, so the initial goal was achieved.

Furthermore, I succeeded in adding a few things outside the scope of the tutorial:
	I implemented basic menus to allow the player to choose to play alone or with a friend.
	If playing alone, I added in a way to adjust the difficulty of the computer player.
		The computer will always try and keep the center of their paddle aligned with the ball. 
		Each frame, the computer rolls 0-100. If this value is greater than the difficulty setting number, it will move.
		The higher the difficulty, the lower the setting number (40, 20, 0 for easy, medium, hard, respectively).
		Effectively, an easy computer has a 60% chance to move each frame, while hard will always move.
		It's not clever, and this leads to stuttering on lower difficulties, but I'm okay with that for this project.
	Added different vertical speeds for the ball depending on where the ball hits the paddle. 
		The tutorial implements a random-ish vertical speed, mine will have no vertical speed if the ball hits dead-center, 
		increasing to a maximum vertical speed the closer to the edge.
	The tutorial only plays 1 game at a time. I implemented a best-of-5 system and the ability to return to menu after a game ends.