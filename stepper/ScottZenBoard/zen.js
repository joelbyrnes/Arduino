
var ZenBoard = function(cssId) {
    this.cssId = cssId;
    this.clusters = [];
    this.timeFactor = 1;
    this.cycleReverseCount = 3;
    this.rotateDuration = 500;
    this.run = false;
}

ZenBoard.prototype.init = function() {
    $(this.cssId + ' .tile.rot90').each(function () {
        rotateTiles($(this), Math.PI * 1.5, 100);
    });
    $(this.cssId + ' .tile.rot180').each(function () {
        rotateTiles($(this), Math.PI * 1, 100);
    });
    $(this.cssId + ' .tile.rot270').each(function () {
        rotateTiles($(this), Math.PI * 0.5, 100);
    });
}

ZenBoard.prototype.start = function() {
    if (this.run) {
        console.log("start called but already running, ignoring");
        return;
    }
    console.log("start, scheduling next movements")
    this.run = true;
    // schedule first set of rotations
    $.each(this.clusters, function (index, cluster) {
        cluster.schedule();
    });
}

ZenBoard.prototype.pause = function() {
    this.run = false;
    $.each(this.clusters, function (index, cluster) {
        cluster.timer.pause();
    });
    console.log("paused, will not iterate further movements until start called")
}

ZenBoard.prototype.resume = function() {
    this.run = true;
    console.log("resuming")

    $.each(this.clusters, function (index, cluster) {
        cluster.timer.resume();
    });
}

ZenBoard.prototype.addSequence = function(groupClass, sequence, zenboard) {
    var cluster = new StepperCluster(groupClass, sequence, this);
    this.clusters.push(cluster);
}

ZenBoard.prototype.tileImageSrc = function(imgUrl) {
    $(this.cssId + " img.tile").attr('src', imgUrl);
    console.log("changed tile image to " + imgUrl)
}

function rotateTiles(id, byRads, duration) {
    $(id).each(function () {
        var current = $(this).data('rotation') || 0;
        var target = current + byRads;
//        console.log($(this).attr('id') + ": " + current + " -> " + target);
//        console.log("byRads: " + byRads + ", movement is by " + Math.abs(current - target));

        snabbt($(this), {
            rotation: [0, 0, target],
            easing: 'ease',
            duration: duration || 500
        });

        $(this).data('rotation', target);
    });
}

// TODO convert to prototype mode. use composition?
//var Timer = function(callback, delay) {
//
//}

// http://stackoverflow.com/questions/3969475/javascript-pause-settimeout
function Timer(callback, delay) {
    var timerId, start, remaining = delay;

    // TODO check for already paused
    this.pause = function() {
        window.clearTimeout(timerId);
        remaining -= new Date() - start;
        console.log("Timer " + timerId + " paused with " + remaining + " remaining")
    };

    // TODO check for already running
    this.resume = function() {
        start = new Date();
        window.clearTimeout(timerId);
        timerId = window.setTimeout(callback, remaining);
        console.log("Timer " + timerId + " started/resumed with " + remaining + " remaining")
    };

    this.resume();
}

var StepperCluster = function(groupClass, sequence, zenboard) {
  this.groupClass = groupClass;
  this.sequence = sequence;
  this.zenboard = zenboard;
  this.seqPos = 0;
  this.timer;
};

StepperCluster.prototype.rotateGroup = function(rads) {
    // clockwise turn in radians is negative
    var rads = rads || Math.PI * -0.5;
    var direction = Math.floor(this.seqPos / (7 * this.zenboard.cycleReverseCount)) % 2 > 0? -1: 1;
//    var direction = 1;

    console.log(this.groupClass + " rotating in direction " + direction);

    rotateTiles(this.groupClass + ":not(.reverse)", rads * direction, this.zenboard.rotateDuration);
    rotateTiles(this.groupClass + ".reverse", -rads * direction, this.zenboard.rotateDuration);
}

StepperCluster.prototype.iterate = function() {
    if (this.zenboard.run) {
        this.rotateGroup();
        this.schedule();
    }
}

StepperCluster.prototype.schedule = function() {
//    console.log(this.groupClass + " timefactor " + this.zenboard.timeFactor);
    var interval = this.sequence[this.seqPos % 7] * this.zenboard.timeFactor * 1000;
    var foo = this;
    this.timer = new Timer(function() {
        foo.iterate();
    }, interval);

    console.log(this.groupClass + " seqPos " + this.seqPos + " (" + this.seqPos % 7 + ")"
            + " scheduled in " + interval + " ms");
    this.seqPos++;
}
