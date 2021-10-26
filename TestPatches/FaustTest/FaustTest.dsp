import("music.lib");

btn1 = checkbox("btn1[OWL:B1]");
led1 = hbargraph("led2>[OWL:B1]", 0, 1);

import("demos.lib");
process = attach(osc(1000) * btn1, 1-btn1 : led1);

// import("all.lib");
// process = zita_light;
