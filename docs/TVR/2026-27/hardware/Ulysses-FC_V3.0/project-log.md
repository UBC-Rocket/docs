# Ulysses-FC_V3.0 Project Log

Running daily log of design decisions, part choices, problems, and lessons.
Capture as you work. Reasoning and lessons inline are the point, not rigid structure.

**Project:** Ulysses-FC V3.0    
**Maintainer:** Gugan Sivakumar    
**Started:** 13/09/2026

Conventions (optional tags for findability later):
- **Important:** flag a technical insight or lesson worth finding again.    
- **Decision:** a choice made (part, approach, value) with the reasoning.    
- **Mistake:** something done wrong and the correction, so it is not repeated.

---

## Open Action Items

Live list of open items. Add here the moment something comes up; remove when when done.

- [ ] <open item>

## Date: 13/09/2026

- We are going for a new FC for TVR because PCIe sucks, very difficult to mount and we are not using half the things in present FC.

- I am just going to desolder the MCU (STM32H745) from present FC and solder it on new FC. Also will share a lot of components with COTS to reduce costs.

- Sensor board is merged with this new FC so all the sensors will be on FC itself. Stack now is FC, backplane and ESC.

- Added MCU footprint to Altium library.

- Finished MCU PWR and Debug schematics.

- Shortlisted a few board to board connectors, will make a decision soon.

- Added a pin assignment doc in the wiki and started work on it.

## Date: 14/09/2026

- Firmware and Controls raised a good point that we have no way to calculate yaw (absolute heading), started looking into that.

- Two main options to do it. First, dual RTK GNSS setup and use carrier phase difference to calculate yaw. Second, more traditional method using a magnetometer to sense earth's magnetic field and calculating yaw.

- Started discussion on NTRIP vs base receiver for RTK error data with firmware, likely going with NTRIP.

## Date: 15/09/2026

- I have decided on a board to board connector, BB51-84AT-1-3HB 84 pin, 1mm pitch, keyed, only about 1.5 CAD and decent stock in LCSC. Pin assignment mostly done as well.

- Firmware confirmed that they are transmitting approx 3.5Kbps now and with this year's design, it may go up to 5Kbps max. So no worries there.

- Finalized radio specs: 915MHz, 300Kbps, GFSK-FHSS, 30dBm transmit power (external PA), approx 140dBm receiver sensitivity, antenna diversity and standard 2dBi antenna.

- 915MHz ISM band transmit power limit is 30dBm and EIRP of 36dBm.

- Still need to figure FHSS details, number of channels and bandwidth.

- Firmware going with NTRIP and some free service for RTCM packets.

## Date: 18/09/2026

- Went down a rabbit hole and had a scare about data rates and transmit power of RFD, comparison with SX1262 and looked at alternative ICs but all good. Just was misunderstanding things.

- Started on a high level block diagram for custom telemetry front end, will take my time with it and calculate/simulate everything.

## Date: 19/09/2026

- Finished IMU schematics, going with ICM-40609-D for now. Firmware and Controls happy with it. Will confirm with Shadab tomorrow.

- RTK schematic, all the config is done. RF front end yet to be finished.

- Good progress on RF front end block diagram, need to iron out a few more high level details and then can start deep dives on each.

- **Important:** Fundamentally, bias tee L needs to have sufficiently high impedance and C needs to have sufficiently low impedance at signal frequency. The SRF peak does not need to be right at signal frequency or even very close to it. Bias tee L and C need to be spec'd so that their SRF is comfortably above signal frequency, especially L can be quite above it as L still gives good impedance at either side of SRF peak until a decent bandwidth. Remember, passive size is a big factor in parasitics. 0402 is standard for RF front end because parasitics are nice and can usually find values required. For Sub-GHz, 0603 can work as well but **SPEC IT PROPERLY**.

- Going with 0603 as much as I can for RF front end, will be careful.

- After getting confused and trying a lot of combinations, finalized bias tee values. Schematic updated with bias tee.

## Date: 20/09/2026

- Added mezzanine connector (BB51-84AT-1-3HB) to Altium library, unfortunately no CAD model available for socket.

- Finished the mezzanine schematic according to the pin assignment doc, made a few changes to pin assignment, just aesthetic stuff.

- Talked to Alex about sensors on present FC, he agrees that sensors are pretty good, nothing wrong with the hardware or model, likely driver mistake or we never tested it properly.

- **Decision:** Going with BMP585 for baro, latest from Bosch, gel filled, more than enough pressure range, should give resolution around 0.5m or so, only 3.6 CAD and good stock in LCSC. Confirmed with Shadab as well.

- After a lot of discussions with Shadab about budget and present FC, we decided that split from present FC is TVR takes the MCUs, COTS takes the BMI IMUs, all the GNSS and baro, and TVR takes the ICM IMUs and TTL.

- Added FDCAN to pin assignment for future linear actuator plan.

- Still need to add VBAT in pin assignment.

## Date: 22/09/2026

- Added VBAT to pin assignment doc, very little pins left. May need to move things around.

- Revised matching circuits, L / T / Pi circuits. Seems like L gives us lowest / fixed Q-factor but widest bandwidth and easiest to implement, Pi gives ability to control Q-factor with the extra capacitor and generally used for high impedance matching (>100R), and T gives the highest Q-factor, ability to control Q and narrow bandwidth, generally used for high quality low impedance matching (<100R).

- **Important:** General implementation of T will AC couple the RF line, so will not work for active antennas or anything with bias tee.

- Decided on L matching for everything except Pi for antenna matching, Semtech seems to recommend the same.

- **Mistake:** Seriously misunderstood matching circuit's design process and fundamentals. Kept on confusing transmission line impedance and matching circuits. **They are not same, very different theory**. Very stupid mistake.

- **Important:** Fundamental principle of matching circuit is **that impedance of a series branch can be equal to impedance of a parallel branch at a frequency**. And imagine the circuit, the impedance of series branch is just R_Source + X_SeriesElement and impedance of parallel branch is just R_Load // X_ParallelElement.

- Q factor for reactive elements is defined as reactive power / resistance power (energy stored / energy lost). That is a key formula. Due to **maximum power transfer theorem, which states that for max power transfer, they need to be conjugate matched (to cancel out reactance), math leads to |Qs| = |Qp| = |Q| with Qs = Xs / Rs and Qp = Rp / Xp**. V^2/R in parallel case inverses the reactance and resistance terms. Finally **Q = (R_high / R_low - 1)^0.5**.

- **Important:** It is not possible to design a matching circuit without knowing source and load resistances.

- **Design process for L matching circuit is just calculate the Q, and use Qs and Qp formula to solve for reactance, substitute frequency to get L and C values**. It will be a **unique set of values**.

- Refer to "Microwave and RF Design III - Networks" in reference resources, full derivation is there and it is very good.

- **Important:** Use an array of capacitors to create required matching capacitance if not standard value.

- Interface schematic almost done.

- Flash schematic done.

## Date: 23/09/2026

- Learnt about RF PA vs gain blocks, gain blocks for ideally high impedance signals, prioritize signal conditioning over gain and they provide a max gain of about 30dB generally so suited if only a small amount of gain is needed. PAs maximise gain even at the cost of matching or signal conditioning, they are rare to find under like 10dB gain but can provide very high gain.

- We only need like 10 to 15dB external gain as RFIC itself can do 22dB so gain block seems better choice and easy to find in LCSC.

- Learnt about noise figure, general NF range for gain block ICs and signal chain noise figure analysis.

- **Decision:** Looked through TI gain block products, decided on TRF37D73 gain block IC because it provides about 12.2dB gain at 915MHz, noise figure of 3.25dB lowest TI offers for our use case, only 1.5 CAD and good stock in LCSC.

- **Decision:** Decided on SKY13370-374LF RF switch for Tx/Rx switch and antenna diversity switch. SPDT, 50R terminated, dual pin control, +39dBm IP1dB, isolation of approx 35dB @ 915MHz, very decent.

- Finished USB-C PWR schematic.

- Ken had an idea to flash the MCU over USB using STM bootloader but David doesn't prefer that. So sticking with ST-Link and no data on USB-C.

- Some progress on MCU schematic.

## Date: 24/09/2026

- Took day off, so all day worked on FC.

## Date: 26/09/2026

- Some progress on schematics.

- Spent most of time in preparation for build day.

## Date: 27/09/2026

- Had a discussion with Ken on all connectors on backplane, and decided on MaUWB, RFD and LA_PWM (DNP) on FC and rest on backplane and eventually on ESC.

- Question on battery connector harness still remains undecided. Likely have to use three parallel XT60 like before until custom ESC is ready.

- Some progress on FC schematic and ioc. Getting there.

## Date: 02/10/2026

- Finished RFD connector schematic.

- Read up on CAN, termination etc.

- **Decision:** Going with TCAN1044 transceiver because FDCAN, standard features, good stock and cheap in LCSC. Finished CAN schematic.

- Added a high level documentation schematic as well. Did some stuff for aesthetics.

- **Decision:** Going with MMC5983MA for mag. Same as previous FC, best I could find as well. Finished mag schematic.

- Testing planned for mag and some mu metal shielding can help.

## Date: 03/10/2026

- Except radio, all the schematics are done.

- Started component placement, antenna interference reduction is implemented by placing radio antenna on end and GNSS antenna on the other end.

- Had a discussion with Controls on IMU placement, consensus was to place it as close to COG as possible which in this case is just COM and all three very close to each other.

- All RF is on bottom layer for some isolation.

- Added RF shield footprint to our library, going with 16mm * 16mm approx for now.

- Updated LiDAR PCB to vertical JST as well.

- Updated mezzanine pinout to make routing easier for backplane.

## Date: 04/10/2026

- Had system design sync for FC and backplane, got cooked.

Main suggestions are:

Sensor raw data validation.

Antenna interference was raised, 915MHz and GNSS antenna is placed as far away as possible for some isolation.

Refer RC drones / open source designs for sensor selection, especially heading.

Consider optical flow sensor for position hold (considering for phase 2).

Add a SD card to store raw sensor data.

Design freeze date for flight controller is 18/05/26.

- Some progress on radio schematic, started adding the matching network and bias tee components.

- **Mistake:** Made a mistake / better way to do it, realized that Samsung and Yageo both provide parasitics data for their passives. Going to use the manufacturer parasitics data from now on, not SimSurf's. Parasitics vary wildly between manufacturers somehow. My bad.

- Using reactance at the signal frequency in matching network calc hereafter, not pure theoretical calc. Basically calculate the required reactance and use sim data to find caps based on it.

- Played around with Yageo's and Samsung's sim tool, very cool. Redid all the calc with manufacturer's data and updated component values.

- Updated matching values for RFIC RFO to gain block is 11nH and 16pF.

- **Mistake:** Cap for bias tee needs to be updated, target < 5 ohms for cap from now on. Updated bias tee values are 220nH and 33pF. I need to do calculations properly next time. Very stupid mistakes.

- Added footprints for RF switch and gain block.

- Gain block schematic is done.

## Date: 10/10/2026

- Finished SD-CARD schematic, added some missing bulk caps and did a schematic review. Looks okay for now.

- Finalized ioc file after consulting with David.

- A lot of soldering practice, getting better but need more practice.

## Date: 11/10/2026

- Almost done with component placement, quick summary is all RF (GNSS and radio) on bottom layer, opposite corners for max separation. All IMU centered around COG / COM and close to each other in bottom layer. Buzzer on top and mag on top as well but opposite side for max separation. Radio SMAs 45 deg rotated to make routing easier and all SMA on top layer, vertical and THT. Everything that needs 12V is on one corner, near to mezzanine 12V, plan is to use a single polygon for 12V. CAN on bottom, because space in bottom.

- JSTs are too bulky, so switching to Pico-Lock series. 1.5mm, 3A, surprisingly stocked and very cheap in LCSC.

- **Important:** Jason asked a good question about load line matching, realized that RFO impedance will change based on transmit power so shouldn't it be load line matched? Quick research suggests that ideally yes but still it is not done often because matching to highest power still works because below that it will have power loss but will be small and the power dependence impedance is only a part of total impedance so efficiency will drop down to like 70% at lower power.

## Date: 12/10/2026

- Finished high level component placement and most low level component placement.

- Do not know why but board does not look pretty at all, not like the picture I had in mind. Feels like something is missing.

- Can shrink the board down by 5-10mm, maybe that will help.

## Date: 13/10/2026

- Most of fine component placement done, ready for routing.

- Moved CAN to bottom because need some straight space, buzzer to a corner for isolation from all sensors, baro on corner and mag on another for better use of space, silkscreen done with logo, GNSS front end layout with shielding and stitching vias plus GND polygon around it in all layers and vias between the GND pads as well for shorten return path.

- Moved a few connectors around as well.

- Looks better now, better use of space, more consistent spacing and better placement.

## Date: 14/10/2026

- Reviewed yesterday's work, looks okay for now.

- Looked through open sourced designs similar to mine, especially RF front ends. Still wondering if a SAW/LNA or at least a SAW needed for GNSS because of radio.

- Telemega (GNSS transmitter) schematic is very good resource, they do use a SAW on GNSS. I am adding at least a SAW on GNSS, still thinking if LNA is needed.

- **Decision:** Decided on SAFEA1G58KA0F00R15 for SAW, made for L1/GLONASS (faster RTK fix due to access to GLONASS satellites as well), 1.6dB insertion loss, 32dB min rejection @ 915MHz, good stock and surprisingly very cheap in LCSC.

- Prefer QPQ1060 because it is temperature compensated but LCSC does not stock Qorvo SAWs. Sad.

## Date: 16/10/2026

- Took day off from work, need to lock in to finish routing.

- GNSS schematic finished with SAW added, found an error in it as well, used Uni-D TVS for GNSS SMA, remember RF is a sinusoid so bipolar and no DC bias as well. Switched to Bi-Di TVS.

- Redid GNSS placement and routing, it was not easy at all, not able to fit all the components in straight line, have to curve somewhere.

- After playing around with it for some time, decided on about a 30 degree rotation of SMA and rotate the components as well plus curve the trace about 30-40 degree, best I could do. Looked through open source RF designs, curving seems fine as long as done properly.

- Finished some general routing, slowly losing my fear of vias and prioritizing short and straight trace path and maintaining spacing and proper trace structure while routing.

- Trying to imagine how I am going to route while doing placement to prevent redoing placement while routing and have a proper routing strategy.

- Prioritizing more sensitive / noisy nets first and route the rest after that.

- Stuck on how to do GND for 12V pads in mezzanine without cutting the 12V polygon, will think about it.

## Date: 17/10/2026

-

## Date: 23/10/2026

- Gain block spice model from Annie was better than what I had, using hers.

- Something is broken in spice, S21 is like -120dB and for some reason +3V3 is disabled, no operating point.

- It was just LTSpice being stupid, removed everything and added again, it works now.

- Apparently .net can do input and output impedance as well, very useful.

- Another way to do it is inject a 1 AC current source into the port of interest and terminate other end, plot V_Port (=Z).

## Date: 24/10/2026

- Locking in today, simulating each filter separately and going to tune each component.

- Zout of 1st harmonic Pi filter is not anywhere near 50R, seems it is not easy to do a 2nd order LPF with specific impedance out because of the frequency response. Trying some things.

- Methodology is choose a corner frequency well above signal frequency and check at what frequency, output is 50R. If it is above sig freq, then shift the whole freq response to left by decreasing the corner frequency and vice-versa, that is how you tune lower order filters to output a particular impedance.

- Much better result now on L LPF now, doing same for Pi LPF.

- **Important:** Always calculate X needed of each component, not component value. Way more accurate.

- **Important:** Remember, symmetric Pi-filter corner freq is 1/(pi*(LC)^0.5), have to take total capacitance and input Z will be equal to output Z if symmetric so no impedance conversion.

- **Important:** Always simulate the impedance of each section and model the output reactance for next section / cancel it out.

- Finally the thing is starting to work, yay.

- Getting the hang of it, starting to make some sense, very happy.

- Cancelling out the reactance from the Pi-filter with a series cap, helps a lot.

- **Important:** Real part of impedance depends primarily on the impedance transformation which primarily depends on Q-factor (=X/R). So to tune real part of impedance, modify the Q-factor. There is no way to tune reactance or real part of impedance independently, both will be affected because it's the ratio that matters.

- **Important:** The shunt element is the tuning knob for real part. Increase X of shunt element to decrease Q factor meaning less transformation (Q = R_High / X_C). If high R to low R, then real part of Zout goes higher.

## Date: 25/10/2026

## Date: 28/10/2026

- Spending some time on the FEM, very little resources available, datasheet is not very detailed.

- No .s2p or spice file available but FEM is very large signal IC so makes sense.

- Wrote a simple black-box spice model for it using the S parameter in datasheet, with a CTRL pin to switch between Tx and Rx chain, just 50R terminated ports with gain calculated from its S21 / S12.

- **Important:** There is a big difference between maximum power transfer and most efficient power transfer. Conjugate match results in most power transfer from source to load but it always halfs the voltage due to voltage divider using R_source and R_load so power becomes 1/4th (-6dB). That is a universal property. Most efficient power transfer will be when load impedance >>>> source impedance which basically maximizes Vout of the voltage divider (voltage divider ratio) but due to high impedance of load, only a small amount of power can be transferred but happens very efficiently. Finally got it. Have been misunderstanding this for so long. Mind blowing.

- **Important:** Always compensate for the halving of voltage at every matched port while modeling it, like ANT or TR pin in FEM.

- **Important:** Remember source needs series termination and load needs parallel termination resistor.

- Spice model seems to be working fine, of course no frequency behaviour. Only operational at 915MHz and purely for S Parameters and impedance measurements.

- FEM only needs bias tees and decoupling caps. No external filters or match needed. Present chain is largely same. Updated HDL.

- Since we will only be using very low power levels from SX1262, Zopt changes significantly but Semtech only gave load pull at +14dBm and +22dBm. So need to do some extrapolation. (Approx is okay, match not super important at low power levels.) Looking into Cripps load line theory which provides a way to predict output power.

- Cripps paper is amazing. Very interesting read. Still working on it but will calculate Zopt at -3dBm (max we need) soon.

## Date: 29/10/2026

- Looked through some open source designs with mixed signal, Dig + RF etc. for some routing inspiration. Hades from Phil's Lab is pretty decent.

- Some work on spice sims after FEM IC added in.

- **Important:** Order of filter is the amount of knobs you can control / variables so only that many characteristics of a filter (shape, corner, Z @ freq etc.) can be controlled as well so a second order filter cannot be designed to have a particular shape, a specific corner freq and a specific Z @ freq.

- **Important:** Z of transmission line is characteristic impedance which in ideal case does not depend on frequency. The reactance is cancelled out by L and C. Same applies to filters as well.

## Date: 30/11/2026

- Spent most of the time on VRNDA motor driver PCB.

- Did some work on spice sims with FEM, mostly done and tuning going good as well.

## Date: 01/11/2026

- Played around with another PA IC Annie found, its spice model is not working for some reason, very expensive and needs external filters to work so cannot accurately characterize how it will interact with SX1262 recommended filters. Sticking with the FEM IC.

- **Mistake:** Front end is too long to do a proper layout inside shield, big mistake using 0603's. Will never do it again.

- Switching all passives to 0402 and 0201's and thinking of getting only the radio section assembled.

- Spice updated with 0402/0201 passives and tuned again. All good.

- Updated about half of the front end in Altium with new smaller passives.

## Date: 02/11/2026

- Made a radio only BOM and CPL, it costs 175 CAD extra for assembling radio section alone. Crazy expensive. Mainly because JLC economic assembly is only up to 6 layers. No way I am doing that.

- Thinking of doing a more rectangular shield with a bit more length and smaller width for more layout space, looked through some shields in TE and LCSC. No good option that can work.

- Switching to 0402 for all RF, going to optimize the layout and add some curves. Try and fit the chain in. Sticking with the shield as well. Assembly in-house.

- Reading up on 90 degree traces, especially for RF (Altium article and one on Google is good). Main takeaway is there is not much concrete proof that 90 deg traces present as partial discontinuity and that it has significant impact on impedance or EMI until like 10's GHz. Of course only use if need to and curve it.

- Updated spice sims with all 0402, some tuning but all good.

- Played round with RF layout a bit to see if anything works, some progress but nothing solid.

## Date: 03/11/2026

- Some more playing around with layout, got a preliminary layout done, looks okay but a few things not liking: clearances are really tight, very constrained layout, still need to think about SI etc.

- Read up on Semtech's PCB design guidelines for SX/LR chips, some decent info there especially about DC-DC switching noise coupling and optimizations. Main takeaways is some clearance for all components to shield recommended, 1.5mm clearance between the DC-DC inductor and everything, and generally think about noise coupling.

- Did some routing and layout, still problems unsolved. Needs more work.

## Date: 04/11/2026

- Did a quick review of the routing in sensitive places, changes:

Add some solder mask exp on impedance all RF traces to reduce insertion loss due to solder mask, keep mask near TVS or any other component.

Via jump CAN diff pair for mezzanine carefully, add a few GND vias around transition to keep field contained and remove GND between all diff pair to keep the coupling within the pair.

Edit rules to add some good sliver for any Z controlled traces to reduce field coupling with same layer pour. Think about return paths, no cutout or anything stupid on reference plane.

Highly advised to use some decent sliver for all, makes rework safer.

Recheck impedance profiles, and modify since need to be via jumped now.

- **Important:** Remove poly between a diff pair to keep the coupling within the pair, some clearance between a pair and poly is also recommended to reduce field coupling with same layer poly which can change its characteristic impedance, we want most of field to terminate in its reference plane. Careful not to put any poly cutouts in reference plane, return path will break and going to radiate all that field out resulting in EMI.

- Worked out some solutions for routing problems yesterday as well:

Move the whole chain inside shield up a bit and move ST-Link down, 4x4 thermal vias on RFIC possible. TCXO also gets more clearance.

Rotate / move a couple of caps near the MCU, thermal vias possible on FEM IC.

12V-GND can be done, just a single poly for GND next to each other, will cut 12V poly but the other side will handle it. Add some vias for the GND poly.

- Corrected impedance profiles with correct Dk's from JLC, trace widths updated. Note cannot route D120 on Layer 6 (width = about 0.087mm).

- Moved the DC-DC inductor and bulk cap for radio to top layer because makes layout much nicer, and reduces coupling of switching noise to shield and other components nearby. (Semtech recommends as well.)

- Implemented above mentioned solutions, all good. TCXO layout is decent and thermal vias on radio IC and FEM IC is good.

- CAN diff pair via jumped to layer 4, layout looks okay. Length difference is about 0.13mm, not bad.

- Added clearance between diff pair to remove poly between them and some clearance to same layer poly as well.

- Added some extra poly cutout where needed and started adding shielding vias as well.

- Most decoupling and bulk cap placed and routed, looks decent but a bit constrained layout.

- Shield position set to 14.4mm and 38.25mm, provides some clearance to all components.

- Moved some traces on top and PWR layer to prioritize RF.

- RF front end layout mostly done.

## Date: 05/11/2026

- Quick review of yesterday's routing:

+3V3 via for Pin 1 on SX1262 needs to be moved, hitting shield pads.

FEM IC Pin 35 decoupling cap can be placed by rotating Pin 1 cap 90 deg and move it a bit.

Move C107 a bit out for some clearance.

Do not forget teardrops on all CI traces and can use thermal relief on decoupling and bulk cap in radio section. (Semtech also agrees.)

Remove thermal relief on any +12V hole / pad (J14).

Why did you remove thermal vias on SX1262 EP? Stupid, add it back.

Pin 24 on SX1262 not routed.

Length of mezzanine CAN diff pair could be a bit more matched (at around 0.6mm diff now), not super important. (Remember Rick Hartley lecture on diff pairs.)

Increase VBAT trace width just to be safe.

Don't forget the 12V-GND thing.

## Date: 06/11/2026

- Most of routing and layout done, doing some touchups now and will start on silkscreen and DRC soon.

- Fixed all the pending changes.

## Date: 07/11/2026

- Read up more on coplanar waveguide routing, and corrected the impedance profile to coplanar. Defined proper V-to-V spacing and V-to-T spacing. Added proper shielding vias with a poly cutout for a defined trace to same layer pour spacing.

- Implemented changes as per Shadab's review, discussion scheduled tomorrow for splitting the board.

## Date: 08/11/2026

- **Mistake:** After a lot of discussions, sticking with the present board. My mistake, should have brought this up long before and had a proper discussion.

- Per Annie's suggestion, moved +12V to backplane.

- Spent most of the time just reviewing it.

## Date: 09/11/2026

- Looks like the stack can be mounted with some optimizations, not ideal but works. My mistake, should have coordinated with mech long before.

- Alex agreed to review it (yay). Looking forward to his suggestions.

## Date: 10/11/2026

- Alex had a lot of useful suggestions, mostly related to DFT / DFM. Will start implementing them.

- **Important:** Some very good practices to follow in future as well like 0R on interfaces, makes testing / debugging easier. 10R/100R (some value) on signal lines to use as jumper and acts as a low pass filter limiting the rise time, reduces overshoot and very good from EMI perspective. Add more notes and context, assuming somebody who knows nothing about the board is reviewing, they should be able to understand.

## Date: 24/11/2026

- Did some post fab review of FC, unfortunately found two mistakes, not critical but still sad though. Documented them. Will keep on reviewing it.