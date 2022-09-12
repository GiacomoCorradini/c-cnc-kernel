//    ____             __ _                       _   _             
//   / ___|___  _ __  / _(_) __ _ _   _ _ __ __ _| |_(_) ___  _ __  
//  | |   / _ \| '_ \| |_| |/ _` | | | | '__/ _` | __| |/ _ \| '_ \ 
//  | |__| (_) | | | |  _| | (_| | |_| | | | (_| | |_| | (_) | | | |
//   \____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__|_|\___/|_| |_|
//                          |___/                                   

/** generate a new directory necessary to build the project
 * -B = build directory
 * build = name of the build directory
 * -> cmake -Bbuild 
 **/

//    ____                      _ _      
//   / ___|___  _ __ ___  _ __ (_) | ___ 
//  | |   / _ \| '_ ` _ \| '_ \| | |/ _ \
//  | |__| (_) | | | | | | |_) | | |  __/
//   \____\___/|_| |_| |_| .__/|_|_|\___|
//                       |_|             

/** compile the directory
 * 1st build = --build something
 * 2nd build = name of the directory to build
 * -> cmake --build build
 * we can compile in parallel specifing the number of cores
 * -> cmake --build build -j4
 **/

/** compile and also install the products
 * cmake --build build -t install
 * the products are install into products_host when compile
 **/

/** compile the directory build by itself
 * -> make -Cbuild -> compile only file modified
 *  * we can also specify the target
 * -> make -Cbuild help -> give the list of target
 * -> make -Cbuild clean -> clean and after compile all the file
 **/

//    ____                      ____                      _ _      
//   / ___|_ __ ___  ___ ___   / ___|___  _ __ ___  _ __ (_) | ___ 
//  | |   | '__/ _ \/ __/ __| | |   / _ \| '_ ` _ \| '_ \| | |/ _ \
//  | |___| | | (_) \__ \__ \ | |__| (_) | | | | | | |_) | | |  __/
//   \____|_|  \___/|___/___/  \____\___/|_| |_| |_| .__/|_|_|\___|
//                                                 |_|             

/**
 * Cross compilation using armv7a
 * ./armv7a cmake -Bxbuild -> configure the project
 * ./armv7a cmake --build xbuild -t install -> compile and also install the project
 * 
 * Open the container armv7a
 * ./armv7a bah
 **/

//   ____                               
//  |  _ \ ___ _ __ ___   _____   _____ 
//  | |_) / _ \ '_ ` _ \ / _ \ \ / / _ \
//  |  _ <  __/ | | | | | (_) \ V /  __/
//  |_| \_\___|_| |_| |_|\___/ \_/ \___|
                                     
/** Remove directory (when something doesn't work properly)
 * rm -rf build/*
 **/

//   ____       _   
//  / ___|  ___| |_ 
//  \___ \ / _ \ __|
//   ___) |  __/ |_ 
//  |____/ \___|\__|
              
/** Set the architecture of our target
 * ccmake -Bbuild
 * using arrow select TARGET_NAME
 * for beaglebone black write -> armv7a
 **/

//   ____            _                        _             _   
//  | __ ) _ __ ___ | | _____ _ __         ___| |_ __ _ _ __| |_ 
//  |  _ \| '__/ _ \| |/ / _ \ '__|       / __| __/ _` | '__| __|
//  | |_) | | | (_) |   <  __/ | ________ \__ \ || (_| | |  | |_ 
//  |____/|_|  \___/|_|\_\___|_| |______| |___/\__\__,_|_|   \__|
//                                                       

/**
 * If goodies/broker_start give permission denied write:
 * chmod a+x goodies/broker_start
 * goodies/broker_start
 **/

//   _____ ____  __  __ 
//  |  ___/ ___||  \/  |
//  | |_  \___ \| |\/| |
//  |  _|  ___) | |  | |
//  |_|   |____/|_|  |_|
                     
/**
 * gv_fsm -> gives the option to use for a fsm
 * To generate the file .h and .c fot the fsm
 * gv_fsm src/example.dot -o src/_example -k stop
 **/

//   ____                   _      ____                   
//  | __ )  ___  __ _  __ _| | ___| __ )  ___  _ __   ___ 
//  |  _ \ / _ \/ _` |/ _` | |/ _ \  _ \ / _ \| '_ \ / _ \
//  | |_) |  __/ (_| | (_| | |  __/ |_) | (_) | | | |  __/
//  |____/ \___|\__,_|\__, |_|\___|____/ \___/|_| |_|\___|
//                    |___/                               

/**
 * CONNECT
 * ssh debian@beaglebone.local / ssh debian@192.168.7.2
 * 
 * COPY FILE 
 * scp name debian@beaglebone.local:
 * scp -r directory/* debian@beaglebone.local:
 **/

//   __  __    _  _____ _        _    ____  
//  |  \/  |  / \|_   _| |      / \  | __ ) 
//  | |\/| | / _ \ | | | |     / _ \ |  _ \ 
//  | |  | |/ ___ \| | | |___ / ___ \| |_) |
//  |_|  |_/_/   \_\_| |_____/_/   \_\____/ 
                                         
/**
 * OPEN SIMULATION
 * Run -> Cartesian3Dprinter.m
 * Wait until simulink is open and run simulink
 * 
 * GIVE COMMAND VIA MQTT EXPLORER
 * Connect -> c-cnc
 * Topic -> c-cnc/setpoint
 * Command -> {"x":0,"y":0,"z":-100,"rapid":true}
 **/


/*
// -------------------

  //fprintf(stdout, "alpha %f\n\n", acos(block_la_alpha(b)));

  fprintf(stdout,"v_i: %.1f, v_m: %.1f, v_f: %.1f\n", b->prof_la->v_i, b->prof_la->v_m, b->prof_la->v_f);

  fprintf(stdout,"s0: %f, s1: %f, s2: %f ,sf: %f\n", b->prof_la->s_0, b->prof_la->s_1, b->prof_la->s_2, b->prof_la->s_f);

  fprintf(stdout,"a: %.3f, d: %.3f\n", b->prof_la->a, b->prof_la->d);

  fprintf(stdout, "vel type: %d\n", b->rank);

  fprintf(stdout, "dt1: %f\tdtm: %f\tdt2: %f\tdt: %f\n", b->prof_la->dt_1, b->prof_la->dt_m, b->prof_la->dt_2, b->prof_la->dt);

  fprintf(stdout, "k %f\n\n", b->prof_la->k);
  // -------------------
*/