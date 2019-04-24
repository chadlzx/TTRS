$('body').scrollspy({ target: '#navbar-example' });

function drop_Down(id,text){
    var element=document.getElementById(id);
    element.value=text;

    console.log(id,text);

}
function swap() {
    var element1=document.getElementById('inputDepart');
    var element2=document.getElementById('inputDest');
    var tmp=element2.value;
    element2.value=element1.value;
    element1.value=tmp;
}
