import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { MenuOptions } from '../interfaces/menu-options';
import { BluetoothSerial } from '@ionic-native/bluetooth-serial/ngx';

@Injectable({
  providedIn: 'root'
})
export class DataService {

  constructor(private bluetooth: BluetoothSerial,
              private http: HttpClient){}

  getMenuOptions(){
    return this.http.get<MenuOptions[]>('/assets/data/menu.json');
  }

  conectarBluetooth(adress: string){
    this.bluetooth.connect(adress).subscribe();
  }

  enviarDato(comando){
    this.bluetooth.write(comando + 'P');
  }

  recibirDato(){
    return this.bluetooth.readUntil('\n')
                .then(
                  ( data )=>{
                    return data;
                  });
  }


}
