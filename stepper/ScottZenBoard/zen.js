
var StepperCluster = function(groupClass, sequence) {
  this.groupClass = groupClass;
  this.sequence = sequence;
  this.seqPos = 0;
//  console.log('StepperCluster instantiated for ' + this.groupClass + " with " + this.sequence);
};

var clusterA = new StepperCluster('.groupA', [3, 4, 5, 6, 7, 8, 3]);
var clusterB = new StepperCluster('.groupB', [4, 5, 6, 7, 8, 3, 4]);
var clusterC = new StepperCluster('.groupC', [5, 6, 7, 8, 3, 4, 5]);
var clusterD = new StepperCluster('.groupD', [6, 7, 8, 3, 4, 5, 6]);
var clusterE = new StepperCluster('.groupE', [7, 8, 3, 4, 5, 6, 7]);
var clusterF = new StepperCluster('.groupF', [8, 3, 4, 5, 6, 7, 8]);

var clusters = [clusterA, clusterB, clusterC, clusterD, clusterE, clusterF];

// the time between sequence steps, in seconds.
var timeFactor = 1;

// reverse direction after this many cycles
var CYCLE_REVERSE_COUNT = 3

function rotateRad(id, byRads, duration) {
    $(id).each(function () {
        var current = $(this).data('rotation') || 0;
        var target = current + byRads;
        console.log($(this).attr('id') + ": " + current + " -> " + target);
//        console.log("byRads: " + byRads + ", movement is by " + Math.abs(current - target));

        snabbt($(this), {
            rotation: [0, 0, target],
            easing: 'ease',
            duration: duration || 500
        });

        $(this).data('rotation', target);
    });
}

function rotateGroup(cluster, rads) {
    // clockwise turn in radians is negative
    var rads = rads || Math.PI * -0.5;
//    var direction = Math.floor(cluster.seqPos / 7 * CYCLE_REVERSE_COUNT) % 2 > 0? -1: 1;
    var direction = 1;

    console.log(cluster.groupClass + " rotating in direction " + direction);

    rotateRad(cluster.groupClass + ":not(.reverse)", rads * direction);
    rotateRad(cluster.groupClass + ".reverse", -rads * direction);
}

function schedule(cluster) {
    var interval = cluster.sequence[cluster.seqPos % 7] * timeFactor * 1000;
    setTimeout(function(){
        iterate(cluster);
    }, interval);

    console.log(cluster.groupClass + " seqPos " + cluster.seqPos + " (" + cluster.seqPos % 7 + ")"
            + " scheduled in " + interval + " ms");
    cluster.seqPos++;
}

function iterate(cluster) {
    rotateGroup(cluster);
    schedule(cluster);
}

$( document ).ready(function() {
    // init to specified rotations

    $('.rot90').each(function () {
        rotateRad($(this), Math.PI * 1.5, 100)
    });
    $('.rot180').each(function () {
        rotateRad($(this), Math.PI * 1, 100)
    });
    $('.rot270').each(function () {
        rotateRad($(this), Math.PI * 0.5, 100)
    });

    console.log("setting initial schedules");

    // schedule first set of rotations
    $.each(clusters, function (index, cluster) {
        schedule(cluster);
    });

});