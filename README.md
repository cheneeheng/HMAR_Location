# HMAR_Location
----

[01] Initial Commit
- 19/01/2017
- system that clusters locations and creates the sector map connecting the location areas

[02] Updated Commit
- 16/02/2017
- system using saved data is running

[03] Correction Commit
- 17/02/2017
- all array data type changed to vector
- corrrected error in processing boundary and caclculating the average

[04] Data File Commit
- 20/02/2017
- added reading and writing data to file
- changed the way data is stored :  
  Scenes - [scene] - [object - [loc,mov,max,min,const], surface.txt]

[05] Class Commit
- 22/02/2017
- rewritten all data to depend on class graph
- added Verbose to show relevant ouput only

[06] Single Edge Commit
- 23/02/2017
- added function to show data with sectormap
- corrected error in angle calculation
- added VERBOSE to show the values independently for motion and nonmotion predictions

[07] True Label Commit
- 24/02/2017
- added function to give label name based on highest prediction rate

[08] Sector direction check Commit
- 27/02/2017
- added a check for +/- as the equation for calculating angle does not give +/-

[09]
- 06/03/2017
- reworked sectormap to check for variation along curve
- curve is updated based on averaging with previous ones

[09a]
- 06/03/2017
- nested all functions in main
- updated the prediction functions

[09b]
- 07/03/2017
- minor updates
- changed read file to read alphabetically
- minor modification to the prediction functions

[09c]
- 07/03/2017
- rectify change between pc

[10]
- 07/03/2017
- started data recording with 3 scenarios:  
  * cup direct drink
  * cup rest drink
  * sponge clean

[11]
- 08/03/2017
- updates for boundary condition.

[11a]
- 08/03/2017
- evaluations for a few scenarios.

[12]
- 09/03/2017
- updated function and variable names.
- added function to fill up the holes in sectormap by allowing multiple loc_pred during adjustment.

[13]
- 10/03/2017
- added a new location.
- updated to show movement (sliding) for both in and outside the location areas.

[14]
- 13/03/2017
- restructured again the code
- clustering is done separately before learning the sector maps
- given a 0.9999 confidence for inside prediction to allow it to be comapred with the rest.
- INTERESTINGLY : the confidence drops at the beginning before increasing again.
- this is probably due to the fact that the location area is clustered as a sphere dome instead of a point.

[15]
- 14/03/2017
- added 3 new libraries
- moved everything from main into util
- main functions are split into 3 modules
- added a check to prevent prediction from showing 0% confidence by taking the last known prediction

[16]
- 15/03/2017
- reworked the colorcode function
- allow the curve check to look for more sectors instead of just 1
