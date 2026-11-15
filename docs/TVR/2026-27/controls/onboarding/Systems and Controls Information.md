For an overview and basic understanding of systems and controls, please read the following GeeksforGeeks article; they do a pretty good job of explaining what a control system is:

https://www.geeksforgeeks.org/electronics-engineering/control-system/

 

Here’s also an excerpt from LibreTexts that goes into more theoretical detail:

https://eng.libretexts.org/Bookshelves/Introductory_Engineering/EGR_1010%3A_Introduction_to_Engineering_for_Engineers_and_Scientists/14%3A_Fundamentals_of_Engineering/14.14%3A_Signals_and_Systems_(Control_systems)



## The Crux:

- Signals are any inputs and outputs (including “outputs” that feed into another component as an “input”) involved in our system.

- Systems contain all the signals, components, and how they affect one another. We can break systems down into subsystems to help us develop a more detailed understanding of overarching behaviour. 

- Block diagrams help us conceptualize how components interact with one another in the system.

- When modelling a system, the most realistic approach is to go from top-down – everything is a black box. Think about what you are putting into the overall system and what you want out of the system.

- Controls can be applied to systems for error correction, when the actual behaviour of the model deviates from the expected behaviour. The most common approach to error correct is using feedback

- When approaching controls, think about how the subsystems and components affect each other. What are things you can modify? How will the rest of the system respond? 



## Final Thoughts:

- Modelling the system will involve lots of mathematics and physics! Do as much research as you can to get a great grasp on the system.

- There are many ways we can go about controlling our system – the most common method of controls is using PID (Proportional, Integrative, Derivative) controllers. For complex systems (such as a Quasi-Monorotor Thrust Vectoring Rocket Drone), multiple PID controllers may need to be used to optimize rocket control.

