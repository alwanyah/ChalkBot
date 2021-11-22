# GPS for ChalkBot

## SparkFun, GPS-RTK Board, NEO-M8P-2
* A very extensove documentation for the  
  https://www.berrybase.de/sensoren-module/funk-kommunikation/sparkfun-qwiic-gps-rtk-board-neo-m8p-2

* https://learn.sparkfun.com/tutorials/gps-rtk-hookup-guide

* Arduino library (with examples): **SparkFun u-blox GNSS**
  https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library

* get started with the u-Center  
  https://learn.sparkfun.com/tutorials/getting-started-with-u-center-for-u-blox/all

## GPS-RTK (Real Time Kinematics)
* BRIEF: the precision of localization can be < 2cm with additional correction data
* What is GPS RTK?
  https://learn.sparkfun.com/tutorials/what-is-gps-rtk
* RTK Standards
  https://gssc.esa.int/navipedia/index.php/RTK_Standards
* Correction Data Format: **RTCM 3.x**


## Terminology
* GNSS - Global Navigation Satellite System
* GPS - Global Positioning System
* RTK - Real Time Kinematics
* RTCM - Radio Technical Commission for Maritime Services
* NTRIP - **N**etworked **T**ransport of **R**TCM via **I**nternet  **P**rotocol
  * https://igs.bkg.bund.de/ntrip/about
  * Software
    https://igs.bkg.bund.de/ntrip/about#ntrip-tools


## SAPOS - Satellitenpositionierungsdienst
* Satellitenpositionierungsdienst in Berlin
  https://www.stadtentwicklung.berlin.de/geoinformation/sapos/

* Software und Anleitung: GnssSurfer, GnssCaster 
    * BRIEF: GnssSurfer ist eine Software zum Abrufen und Verteilen von **RTCM 3.x** Daten über das **NTRIP**
    * http://217.9.43.196/Download

* Zugangskennung für den HEPS-Dienst über Ntrip (bitte vertrailich behandeln)
   * Zugangskennung: saposbln426-1
   * Passwort:   gbm10-40hu

* Mountpoints mit den dazugehörigen Datenformaten stehen zur Auswahl:
    ```
    VRS_3_4G_BE  - RTCM 3.2, VRS (MSM)  
    VRS_3_3G_BE  - RTCM 3.2, VRS (MSM) 
    VRS_3_2G_BE  - RTCM 3.1, VRS
    MAC_3_2G_BE  - RTCM 3.1, MAC
    FKP_3_2G_BE  - RTCM 3.1, FKP
    EPS_BE       - RTCM 2.3
    ```

* Adresse des Casters
    ```
    157.97.109.112:2101
    ```
    oder
    ```
    www.sapos-be-ntrip.de:2101
    ```
* Kontakt
    Jürgen Siebert
    Senatsverwaltung für Stadtentwicklung u. Wohnen
    Abteilung III (Geoinformation)
    Referat IIIB1 (geodätische Referenzsysteme)
    Fehrbelliner Platz 1
    10707 Berlin
    Tel.: +4930/90139-5373
    Fax.: +4930/90139-5361
    
* **NOTE:** *Gemäß den Open Data Prinzipien des Landes Berlin erhalten Sie die Daten geldleistungsfrei.*
