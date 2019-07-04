import { Component, OnInit } from '@angular/core';


@Component({
  selector: 'app-inicio',
  templateUrl: './inicio.page.html',
  styleUrls: ['./inicio.page.scss'],
})
export class InicioPage implements OnInit {

  path_img = '/assets/shapes.svg';
  names = 'NOMBRES';
  dirreccion = 'ISA ADRESS'; 


  constructor() { }

  ngOnInit() {
  }

  onCLick(){
    console.log('falta Implementar');
  }

}
