{ TPLNumberRange }

constructor TPLNumberRange.Create(const AMin, AMax: T);
begin
  min := AMin;
  max := AMax;
end;

class function TPLNumberRange.InRanges(const AValue: T;
  const ARanges: array of TPLNumberRangeT): TPLBool;
var
  i: SizeInt;
begin
  Result := false;

  for i := Low(ARanges) to High(ARanges) do begin
    if (ARanges[i].min <= AValue) and (AValue <= ARanges[i].max) then exit(true);
  end;
end;

function TPLNumberRange.Correct(const ANumber: Variant): T;
begin
  if ANumber < min then Result := min else
  if ANumber > max then Result := max else
  Result := T(ANumber);
end;

{ TPLListEnumerator }

function TPLListEnumerator.GetCurrent: E;
begin
  Result := FList[FIndex];
end;

constructor TPLListEnumerator.Create(const AList: L);
begin
  inherited Create;

  FList := AList;
  FIndex := 0;
end;

function TPLListEnumerator.MoveNext: TPLBool;
begin
  if FIndex+1 < FList.Count then begin
    Result := true;
    Inc(FIndex);
  end else Result := false;
end;

procedure TPLListEnumerator.Reset;
begin
  FIndex := 0;
end;

{ TPLObjectList }

function TPLObjectList.GetItem(AIndex: SizeInt): T;
begin
  if (AIndex < 0) or (AIndex >= FSize) then
    Result := nil
  else
    Result := FArray[AIndex];
end;

procedure TPLObjectList.SetItem(AIndex: SizeInt; AValue: T);
begin
  if (AIndex < 0) or (AIndex >= FSize) then exit;

  FArray[AIndex] := AValue;
end;

function TPLObjectList.DefaultCompare(a, b: T): TPLBool;
begin
  Result := a = b;
end;

procedure TPLObjectList.MergeArray(var AArray: TListOfT; l, r, x, y: TPLInt;
  AComparator: specialize TPLObjectListSortCompare<T>);
var
  i, j, k, s: TPLInt;
  c: TListOfT;
  a: TListOfT absolute AArray;
begin
  i := l;
  j := y;
  k := 0;
  SetLength(c, r - l + 1 + y - x + 1);

  while (l <= r) and (x <= y) do begin
    if AComparator(a[l], a[x]) = -1 then begin
      c[k] := a[l];
      Inc(l);
    end else begin
      c[k] := a[x];
      Inc(x);
    end;
    Inc(k);
  end;

  if l <= r then for s := l to r do begin
    c[k] := a[s];
    Inc(k);
  end else for s := x to y do begin
    c[k] := a[s];
    Inc(k);
  end;

  k := 0;
  for s := i to j do begin
    a[s] := c[k];
    Inc(k);
  end;
end;

procedure TPLObjectList.SortArray(AArray: TListOfT; ALeft, ARight: TPLInt;
  AComparator: specialize TPLObjectListSortCompare<T>);
var
  m: TPLInt;
begin
  if ALeft >= ARight then exit;

  m := (ALeft + ARight) div 2;
  SortArray(AArray, ALeft, m, AComparator);
  SortArray(AArray, m + 1, ARight, AComparator);
  MergeArray(AArray, ALeft, m, m + 1, ARight, AComparator);
end;

function TPLObjectList.GetEnumerator: IObjectListEnumerator;
begin
  Result := TObjectListEnumerator.Create(Self);
end;

function TPLObjectList.Find(AItem: T; AComparator: specialize
  TPLObjectListFindCompare<T>): SizeInt;
var
  i: SizeInt;
begin
  Result := -1;
  if not Assigned(AComparator) then AComparator := @DefaultCompare;

  for i := 0 to FSize-1 do
    if AComparator(FArray[i], AItem) then exit(i);
end;

procedure TPLObjectList.Sort(AComparator: specialize TPLObjectListSortCompare<T>);
begin
  if not Assigned(AComparator) then exit;

  SortArray(FArray, 0, Length(FArray)-1, AComparator);
end;

constructor TPLObjectList.Create(AFreeObjects: TPLBool);
begin
  inherited Create;

  FSize := 0;
  FFreeObjects := AFreeObjects;
  SetLength(FArray, 0);
end;

destructor TPLObjectList.Destroy;
begin
  Clear;

  inherited Destroy;
end;

procedure TPLObjectList.Add(AItem: T);
begin
  if FSize >= MaxListSize then exit;

  SetLength(FArray, FSize + 1);
  FArray[FSize] := AItem;
  FSize += 1;
end;

procedure TPLObjectList.Remove(AItem: T);
var
  id, i: SizeInt;
begin
  id := Find(AItem);

  if id < 0 then exit;
  if FFreeObjects then FArray[id].Free;

  for i := id to FSize-2 do
    FArray[i] := FArray[i + 1];

  FSize := FSize - 1;
  SetLength(FArray, FSize);
end;

function TPLObjectList.Count: SizeInt;
begin
  Result := FSize;
end;

function TPLObjectList.Empty: TPLBool;
begin
  Result := FSize = 0;
end;

procedure TPLObjectList.Clear;
var
  i: integer;
begin
  if FFreeObjects then begin
    for i := 0 to FSize-1 do FArray[i].Free;
  end;

  FSize := 0;
  SetLength(FArray, FSize);
end;

function TPLObjectList.Last: T;
begin
  Result := GetItem(FSize - 1);
end;

function TPLObjectList.First: T;
begin
  Result := GetItem(0);
end;

function TPLObjectList.Duplicate: specialize IPLObjectList<T>;
var
  x: T;
begin
  Result := TPLObjectList.Create(FFreeObjects);

  for x in self do Result.Add(x);
end;

{ TPLList }

function TPLList.GetItem(AIndex: SizeInt): T;
begin
  if (AIndex < 0) or (AIndex >= FSize) then
    Result := Default(T)
  else
    Result := FArray[AIndex];
end;

function TPLList.GetData: Pointer;
begin
  if FSize > 0 then Result := @FArray[0] else Result := nil;
end;

procedure TPLList.SetItem(AIndex: SizeInt; AValue: T);
begin
  if (AIndex < 0) or (AIndex >= FSize) then exit;

  FArray[AIndex] := AValue;
end;

function TPLList.GetEnumerator: IListEnumerator;
begin
  Result := TListEnumerator.Create(Self);
end;

constructor TPLList.Create;
begin
  inherited Create;

  FSize := 0;
  SetLength(FArray, 0);
end;

destructor TPLList.Destroy;
begin
  Clear;

  inherited Destroy;
end;

procedure TPLList.Add(AItem: T);
begin
  if FSize >= MaxListSize then exit;

  SetLength(FArray, FSize + 1);
  FArray[FSize] := AItem;
  FSize += 1;
end;

procedure TPLList.Remove(AItem: T);
var
  id, i: SizeInt;
begin
  id := Find(AItem);

  if id < 0 then exit;

  for i := id to FSize-2 do
    FArray[i] := FArray[i + 1];

  FSize := FSize - 1;
  SetLength(FArray, FSize);
end;

function TPLList.Find(AItem: T): SizeInt;
var
  i: SizeInt;
begin
  Result := -1;
  for i := 0 to FSize-1 do
    if FArray[i] = AItem then exit(i);
end;

function TPLList.Count: SizeInt;
begin
  Result := FSize;
end;

function TPLList.Empty: TPLBool;
begin
  Result := FSize = 0;
end;

procedure TPLList.Clear;
begin
  FSize := 0;
  SetLength(FArray, FSize);
end;

function TPLList.Last: T;
begin
  Result := GetItem(FSize - 1);
end;

function TPLList.First: T;
begin
  Result := GetItem(0);
end;

function TPLList.Duplicate: specialize IPLList<T>;
var
  x: T;
begin
  Result := TPLList.Create;

  for x in self do Result.Add(x);
end;

{ TPLFuncs }

class procedure TPLFuncs.Swap(var a, b: T);
var
  tmp: T;
begin
  tmp := a;
  a := b;
  b := tmp;
end;

class function TPLFuncs.NewArray(tab: array of T): specialize TArray<T>;
var
  i: TPLInt;
begin
  SetLength(Result, Length(tab));
  for i := Low(tab) to High(tab) do
    Result[i] := tab[i];
end;

{ TPLParameter }

class operator TPLParameter.=(a, b: TPLParameter) r: TPLBool;
begin
  r := (a.Key = b.Key) and (a.Value = b.Value);
end;

constructor TPLParameter.Create(AKey: K; AValue: V);
begin
  Key := AKey;
  Value := AValue;
end;

{ TPLHTMLObjectAttributes }

function TPLHTMLObjectAttributes.GetCharset: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[7];
end;

function TPLHTMLObjectAttributes.GetClass: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[0];
end;

function TPLHTMLObjectAttributes.GetHref: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[4];
end;

function TPLHTMLObjectAttributes.GetId: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[2];
end;

function TPLHTMLObjectAttributes.GetName: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[1];
end;

function TPLHTMLObjectAttributes.GetRel: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[8];
end;

function TPLHTMLObjectAttributes.GetSrc: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[5];
end;

function TPLHTMLObjectAttributes.GetStyle: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[3];
end;

function TPLHTMLObjectAttributes.GetType: PPLHTMLObjectAttribute;
begin
  Result := FPtrs[6];
end;

procedure TPLHTMLObjectAttributes.UpdateConsts;
begin
  FPtrs[0] := Get('class');
  FPtrs[1] := Get('name');
  FPtrs[2] := Get('id');
  FPtrs[3] := Get('style');
  FPtrs[4] := Get('href');
  FPtrs[5] := Get('src');
  FPtrs[6] := Get('type');
  FPtrs[7] := Get('charset');
  FPtrs[8] := Get('rel');
end;

procedure TPLHTMLObjectAttributes.Add(AItem: T);
begin
  inherited Add(AItem);
  UpdateConsts;
end;

procedure TPLHTMLObjectAttributes.Remove(AItem: T);
begin
  inherited Remove(AItem);
  UpdateConsts;
end;

procedure TPLHTMLObjectAttributes.Clear;
begin
  inherited Clear;
  UpdateConsts;
end;

function TPLHTMLObjectAttributes.Get(AName: TPLString): PPLHTMLObjectAttribute;
var
  a: TPLHTMLObjectAttribute;
begin
  AName := AName.Trim.ToLower;

  if AName.IsEmpty then exit(nil);

  for a in Self do
    if a.Key.ToLower = AName then exit(PPLHTMLObjectAttribute(@a));

  Result := nil;
end;

function TPLHTMLObjectAttributes.Has(AName: TPLString): TPLBool;
begin
  Result := Assigned(Get(AName));
end;

function TPLHTMLObjectAttributes.ToString: TPLString;
var
  attr: TPLHTMLObjectAttribute;
begin
  Result := '';

  for attr in self do
    Result += attr.Key + '="' + attr.Value + '" ';

  Result := Result.Trim;
end;


