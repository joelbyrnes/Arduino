
var ZenBoard = function(cssId) {
    this.cssId = cssId;
    this.clusters = [];
    this.timeFactor = 1;
    this.cycleReverseCount = 3;
    this.rotateDuration = 500;
}

ZenBoard.prototype.init = function() {
    // TODO should only select within enclosing elem

    $('.rot90').each(function () {
        rotateTiles($(this), Math.PI * 1.5, 100);
    });
    $('.rot180').each(function () {
        rotateTiles($(this), Math.PI * 1, 100);
    });
    $('.rot270').each(function () {
        rotateTiles($(this), Math.PI * 0.5, 100);
    });
}

ZenBoard.prototype.start = function() {
    // schedule first set of rotations
    $.each(this.clusters, function (index, cluster) {
        cluster.schedule();
    });
}

ZenBoard.prototype.addSequence = function(groupClass, sequence, zenboard) {
    var cluster = new StepperCluster(groupClass, sequence, this);
    this.clusters.push(cluster);
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

var StepperCluster = function(groupClass, sequence, zenboard) {
  this.groupClass = groupClass;
  this.sequence = sequence;
  this.zenboard = zenboard;
  this.seqPos = 0;
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
    this.rotateGroup();
    this.schedule();
}

StepperCluster.prototype.schedule = function() {
//    console.log(this.groupClass + " timefactor " + this.zenboard.timeFactor);
    var interval = this.sequence[this.seqPos % 7] * this.zenboard.timeFactor * 1000;
    var foo = this;
    setTimeout(function(){
        foo.iterate();
    }, interval);

    console.log(this.groupClass + " seqPos " + this.seqPos + " (" + this.seqPos % 7 + ")"
            + " scheduled in " + interval + " ms");
    this.seqPos++;
}
