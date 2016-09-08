function setRotation(obj, deg) {
    obj.css('-webkit-transform','rotate(' + deg + 'deg)');
    obj.css('-moz-transform','rotate(' + deg + 'deg)');
    obj.css('transform','rotate(' + deg + 'deg)');
}

function rotateRad(id, rad) {
    $(id).each(function () {
        console.log($(this).attr('id') + ": " + getRotationRadians($(this)) + " -> " + rad);

        snabbt($(this), {
          rotation: [0, 0, rad],
          easing: 'ease'
        });
    });
}

function rotateGroup(groupClass, rads) {
    rotateRad(groupClass + ":not(.reverse)", rads);
    rotateRad(groupClass + ".reverse", -rads);
}

function rotateGroupRepeat(groupClass, rads) {
    rotateGroup(groupClass, rads);
    setTimeout(function() {
        rotateGroupRepeat(groupClass, rads + Math.PI * -0.5);
    }, 1000);
}

// some genius at http://stackoverflow.com/questions/8270612/get-element-moz-transformrotate-value-in-jquery
function getRotationDegrees(obj) {
    var matrix = obj.css("-webkit-transform") ||
            obj.css("-moz-transform")    ||
            obj.css("-ms-transform")     ||
            obj.css("-o-transform")      ||
            obj.css("transform");
    if(matrix !== 'none') {
        var values = matrix.split('(')[1].split(')')[0].split(',');
        var a = values[0];
        var b = values[1];
        var angle = Math.round(Math.atan2(b, a) * (180/Math.PI));
    } else { var angle = 0; }
    //return (angle < 0) ? angle + 360 : angle;
    return angle;
}

function getRotationRadians(obj) {
    var matrix = obj.css("-webkit-transform") ||
            obj.css("-moz-transform")    ||
            obj.css("-ms-transform")     ||
            obj.css("-o-transform")      ||
            obj.css("transform");
    if(matrix !== 'none') {
        var values = matrix.split('(')[1].split(')')[0].split(',');
        var a = values[0];
        var b = values[1];
        var rads = Math.atan2(b, a);
    } else { var rads = 0; }
    //return (rads < 0) ? rads + (2*Math.PI) : rads;
    return rads;
}

$( document ).ready(function() {
    // init to specified rotations

    $('.rot90').each(function () {
//        setRotation($(this), 90);
    });
    $('.rot180').each(function () {
//        setRotation($(this), 180);
    });
    $('.rot270').each(function () {
//        setRotation($(this), 270);
    });

    setTimeout(function(){
        rotateGroup('.groupA', Math.PI * -0.5);
    }, 500);

    rotateGroupRepeat('.groupB', Math.PI * -0.5);


/*
    var id = '#tile00';

    // why the hell do radians CCW?

    var rads = 0;
    var turn = Math.PI * -0.5;

    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 500);
    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 2500);
    setTimeout(function(){
        rotateRad(id, rads += -turn);
    }, 4500);
    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 6500);
    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 8500);
    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 10500);
    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 12500);
    setTimeout(function(){
        rotateRad(id, rads += turn);
    }, 14500);
    setTimeout(function(){
        rotateRad(id, rads += -turn);
    }, 16500);

*/
//    rotateGroupRepeat('.groupA', 90);
//    rotateGroupRepeat('.groupE', 90);
//    rotateGroupRepeat('.groupF', 90);
//    rotateGroupRepeat('.groupB', 90);
//    rotateGroupRepeat('.groupC', 90);
//    rotateGroupRepeat('.groupD', 90);

    /*
    setTimeout(function(){
        rotateGroup('.groupA', 180);
    }, 500);
    setTimeout(function(){
        rotateGroup('.groupB', 180);
    }, 1000);
    setTimeout(function(){
        rotateGroup('.groupC', 180);
    }, 1500);
    setTimeout(function(){
        rotateGroup('.groupD', 180);
    }, 2000);
    setTimeout(function(){
        rotateGroup('.groupE', 180);
    }, 2500);
    setTimeout(function(){
        rotateGroup('.groupF', 180);
    }, 3000);
    */

});