function setRotation(obj, deg) {
    obj.css('-webkit-transform','rotate(' + deg + 'deg)');
    obj.css('-moz-transform','rotate(' + deg + 'deg)');
    obj.css('transform','rotate(' + deg + 'deg)');
}

function rotate(id, deg) {
    $(id).each(function () {
        var current = getRotationDegrees($(this))
        current = (current < 0) ? current + 360 : current;

        // avoid spinning from 360 back to 90 when we want 0 to 90
        if (deg < 0 && (current == 360 || current == 0)) {
            //current = 0;
            //setRotation($(this), 0);
        }

        var target = current + deg;
        console.log($(this).attr('id') + ": " + current + " -> " + target);
        $(this).animate({ degrees: target}, {
            step: function(now, fx) {
                console.log($(this).attr('id') + ": " + now);
                //setRotation($(this), deg);
                $(this).css('-webkit-transform','rotate('+now+'deg)');
                $(this).css('-moz-transform','rotate('+now+'deg)');
                $(this).css('transform','rotate('+now+'deg)');
            },
            duration:'slow'
        }, 'linear');
    });
}

function rotateGroup(groupClass, deg) {
    rotate(groupClass + ":not(.reverse)", deg);
    rotate(groupClass + ".reverse", -deg);
}

function rotateGroupRepeat(groupClass, deg) {
    rotateGroup(groupClass, deg);
    setTimeout(function() {
        rotateGroupRepeat(groupClass, deg + 90);
    }, 500);
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

$( document ).ready(function() {
    // init to specified rotations

    $('.rot90').each(function () {
        setRotation($(this), 90);
    });
    $('.rot180').each(function () {
        setRotation($(this), 180);
    });
    $('.rot270').each(function () {
        setRotation($(this), 270);
    });

    var id = '#tile00';

    setTimeout(function(){
        rotate(id, 90);
    }, 500);
    setTimeout(function(){
        rotate(id, 90);
    }, 2500);
    setTimeout(function(){
        rotate(id, 90);
    }, 4500);
    setTimeout(function(){
        rotate(id, 90);
    }, 6500);
    setTimeout(function(){
        rotate(id, 90);
    }, 8500);
    setTimeout(function(){
        rotate(id, 90);
    }, 10500);

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