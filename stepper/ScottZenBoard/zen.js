
var StepperCluster = function(groupClass, sequence, zenboard) {
  this.groupClass = groupClass;
  this.sequence = sequence;
  this.seqPos = 0;
  this.zenboard = zenboard;
//  console.log('StepperCluster instantiated for ' + this.groupClass + " with " + this.sequence);
};

var ZenBoard = function(elem, clusters) {
    this.elem = elem;
    this.clusters = clusters;
    this.timeFactor = 1;
    this.cycleReverseCount = 3
}

ZenBoard.prototype.init = function() {
    // TODO should only select within enclosing elem

    $('.rot90').each(function () {
        rotateRad($(this), Math.PI * 1.5, 100);
    });
    $('.rot180').each(function () {
        rotateRad($(this), Math.PI * 1, 100);
    });
    $('.rot270').each(function () {
        rotateRad($(this), Math.PI * 0.5, 100);
    });
}

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

StepperCluster.prototype.rotateGroup = function(rads) {
    // clockwise turn in radians is negative
    var rads = rads || Math.PI * -0.5;
//    var direction = Math.floor(this.seqPos / 7 * CYCLE_REVERSE_COUNT) % 2 > 0? -1: 1;
    var direction = 1;

    console.log(this.groupClass + " rotating in direction " + direction);

    rotateRad(this.groupClass + ":not(.reverse)", rads * direction);
    rotateRad(this.groupClass + ".reverse", -rads * direction);
}

StepperCluster.prototype.iterate = function() {
    this.rotateGroup();
    this.schedule();
}

StepperCluster.prototype.schedule = function() {
    console.log(this.groupClass + " timefactor " + this.zenboard.timeFactor);
    var interval = this.sequence[this.seqPos % 7] * this.zenboard.timeFactor * 1000;
    var foo = this;
    setTimeout(function(){
        foo.iterate();
    }, interval);

    console.log(this.groupClass + " seqPos " + this.seqPos + " (" + this.seqPos % 7 + ")"
            + " scheduled in " + interval + " ms");
    this.seqPos++;
}

ZenBoard.prototype.start = function() {
    // schedule first set of rotations
    var board = this;
    $.each(this.clusters, function (index, cluster) {
        cluster.zenboard = board;
        cluster.schedule();
    });
}

// TODO move below to HTML page
var board;

$( document ).ready(function() {
    var clusterA = new StepperCluster('.groupA', [3, 4, 5, 6, 7, 8, 3]);
    var clusterB = new StepperCluster('.groupB', [4, 5, 6, 7, 8, 3, 4]);
    var clusterC = new StepperCluster('.groupC', [5, 6, 7, 8, 3, 4, 5]);
    var clusterD = new StepperCluster('.groupD', [6, 7, 8, 3, 4, 5, 6]);
    var clusterE = new StepperCluster('.groupE', [7, 8, 3, 4, 5, 6, 7]);
    var clusterF = new StepperCluster('.groupF', [8, 3, 4, 5, 6, 7, 8]);

    var clusters = [clusterA, clusterB, clusterC, clusterD, clusterE, clusterF];

    board = new ZenBoard($('#zenboard'), clusters);

    // the time between sequence steps, in seconds.
    board.timeFactor = 1;

    // reverse direction after this many cycles
    board.cycleReverseCount = 3

    // init to specified rotations
    board.init();

    console.log("setting initial schedules");
    board.start();
});