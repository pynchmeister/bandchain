module Styles = {
  open Css;

  let container = style([padding(`px(20)), background(Colors.lighterGray)]);

  let paramsContainer = style([display(`inlineBlock)]);

  let listContainer =
    style([
      display(`grid),
      gridColumnGap(`px(15)),
      gridTemplateColumns([`auto, `px(280)]),
      background(Colors.white),
      border(`px(1), `solid, Colors.lightGray),
      alignItems(`center),
    ]);

  let keyContainer = style([marginLeft(`px(25)), display(`flex), justifyContent(`flexEnd)]);

  let input =
    style([
      width(`percent(100.)),
      background(white),
      padding(Spacing.md),
      paddingLeft(`px(10)),
      fontSize(`px(14)),
      outline(`px(1), `none, white),
    ]);

  let button =
    style([
      width(`px(110)),
      backgroundColor(Colors.btnGreen),
      borderRadius(`px(4)),
      border(`px(0), `solid, Colors.green),
      fontSize(`px(12)),
      fontWeight(`medium),
      color(`hex("127658")),
      cursor(`pointer),
      outline(`px(1), `none, white),
      padding2(~v=Css.px(10), ~h=Css.px(10)),
    ]);
};

let parameterInput = (name, value, updateData) => {
  <div className=Styles.listContainer key=name>
    <div className=Styles.keyContainer> <Text value=name size=Text.Lg /> </div>
    <input
      className=Styles.input
      type_="text"
      value
      placeholder="Input Parameter here"
      onChange={event => {
        let newVal = ReactEvent.Form.target(event)##value;
        updateData(name, newVal);
      }}
    />
  </div>;
};

[@react.component]
let make = (~script: ScriptHook.Script.t) => {
  let params = script.info.params;
  let preData = params->Belt.List.map(({name}) => (name, ""));
  let (data, setData) = React.useState(_ => preData);

  let updateData = (targetName, newVal) => {
    let newData =
      data->Belt.List.map(((name, value)) =>
        if (name == targetName) {
          (name, newVal);
        } else {
          (name, value);
        }
      );
    setData(_ => newData);
  };

  <div className=Styles.container>
    <Text value="Request Data with Parameters" color=Colors.darkGrayText size=Text.Lg />
    <VSpacing size=Spacing.md />
    <div className=Styles.paramsContainer>
      {data
       ->Belt.List.map(((name, value)) => parameterInput(name, value, updateData))
       ->Array.of_list
       ->React.array}
    </div>
    <VSpacing size=Spacing.md />
    <button
      className=Styles.button
      onClick={_ =>
        AxiosRequest.execute(
          AxiosRequest.t(
            ~codeHash={
              script.info.codeHash |> Hash.toHex;
            },
            ~params=Js.Dict.fromList(data),
          ),
        )
      }>
      {"Send Request" |> React.string}
    </button>
  </div>;
};